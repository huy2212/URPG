// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputTriggers.h"
#include "Math/Color.h"
#include "MyProject/MyProjectCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Item/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Character/CharacterState.h"
#include "Animation/AnimMontage.h"

//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;            // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;       // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(
        CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom
                                                      // adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;    // Camera does not rotate relative to arm
    AttackCounter = 0;
    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMyProjectCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();

    // Add Input Mapping Context
    if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent)
{
    if (UEnhancedInputComponent *EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {

        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Look);
        EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Equip);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Attack);
    }
}

void AMyProjectCharacter::Move(const FInputActionValue &Value)
{
    if (ActionState == EActionState::Attacking)
        return;
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AMyProjectCharacter::Look(const FInputActionValue &Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AMyProjectCharacter::Equip()
{
    AWeapon *OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

    if (!OverlappingWeapon)
        return;

    if (EquippedWeapon && EquippedWeapon->WeaponDataAsset)
    {
        EWeaponType OldWeaponType = EquippedWeapon->WeaponDataAsset->WeaponType;
        EWeaponType NewWeaponType = OverlappingWeapon->WeaponDataAsset->WeaponType;
        if (OldWeaponType != EWeaponType::Unequipped)
        {
            if (OldWeaponType != NewWeaponType)
            {
                // If character is holding a weapon, remove it
                UnequippedOldWeapon();
            }
            else if (IsOneWeaponEquipped)
            {
                // This case means that character is trying to hold weapon in another hand
                AnotherHandWeapon = OverlappingWeapon;
                GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, AnotherHandWeapon->GetName());
            }
        }
    }

    // Set new weapon to a hand
    if (!EquippedWeapon)
    {
        EquippedWeapon = OverlappingWeapon;
        IsOneWeaponEquipped = true;
    }
    UWeaponDataAsset *WeaponAsset = OverlappingWeapon->WeaponDataAsset;
    if (WeaponAsset)
    {
        if (WeaponAsset->bIsRightHand)
        {
            OverlappingWeapon->Attach(GetMesh(), WeaponAsset->RightHandSocketName);
        }
        else
        {
            OverlappingWeapon->Attach(GetMesh(), WeaponAsset->LeftHandSocketName);
        }
        switch (WeaponAsset->WeaponType)
        {
        case (EWeaponType::BoxingGlove):
            CharacterState = ECharacterState::BoxingGloveEquipped;
            break;
        case (EWeaponType::LightSword):
            CharacterState = ECharacterState::LightSwordEquipped;
            break;
        default:
            CharacterState = ECharacterState::Unarmed;
        }
    }
}

void AMyProjectCharacter::Attack()
{
    if (ActionState == EActionState::Unoccupied)
    {
        if (!EquippedWeapon || !EquippedWeapon->WeaponDataAsset)
            return;
        if (EquippedWeapon->WeaponDataAsset->WeaponType == EWeaponType::Unequipped)
            return;
        GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, "1");
        PlayAttackMontage();
        ActionState = EActionState::Attacking;
    }
}

void AMyProjectCharacter::PlayAttackMontage()
{
    UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance();
    UAnimMontage *AttackMontage = EquippedWeapon->GetWeaponAttackMontage();
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        AttackCounter = (AttackCounter % AttackMontage->GetNumSections()) + 1;
        FName AttackName = FName(*FString::Printf(TEXT("Attack%d"), AttackCounter));
        AnimInstance->Montage_JumpToSection(AttackName, AttackMontage);
    }
}

void AMyProjectCharacter::AttackEnd()
{
    ActionState = EActionState::Unoccupied;
}

void AMyProjectCharacter::ToggleCollision(ECollisionEnabled::Type CollisionType)
{
    if (EquippedWeapon && EquippedWeapon->GetWeaponHitBox())
    {
        EquippedWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionType);
    }
}

void AMyProjectCharacter::UnequippedOldWeapon()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Detach();
        EquippedWeapon = nullptr;
    }
    if (AnotherHandWeapon)
    {
        AnotherHandWeapon->Detach();
        AnotherHandWeapon = nullptr;
    }
    IsOneWeaponEquipped = false;
}
