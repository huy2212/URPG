#include "MyProject/MyProjectCharacter.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMetaData.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Containers/Array.h"
#include "DataAsset/AttackBehavior.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/PlayerController.h"
#include "HUD/HealthBar.h"
#include "InputTriggers.h"
#include "Math/Color.h"
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
#include "Item/Item.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Animation/AnimMontage.h"

AMyProjectCharacter::AMyProjectCharacter()
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    GetCapsuleComponent()->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);

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
    GetCharacterMovement()->MaxWalkSpeed = 600.f;
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

    HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
}

void AMyProjectCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();

    if (WidgetClass)
    {
        HealthBarWidget = Cast<UHealthBar>(CreateWidget(GetWorld(), WidgetClass));
        if (HealthBarWidget)
        {
            HealthBarWidget->AddToViewport();
        }
    }

    AnimInstance = GetMesh()->GetAnimInstance();

    // Add Input Mapping Context
    if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
    {
        if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    /** Set initial state of character */
    CharacterState = ECharacterState::Unarmed;
    ActionState = EActionState::Unoccupied;
    IsEquippingWeapon = false;
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
        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AMyProjectCharacter::Block);
        EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::BlockEnd);
    }
}

void AMyProjectCharacter::Move(const FInputActionValue &Value)
{
    SetStateToUnequipped();
    if (ActionState == EActionState::Attacking) return;
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

    // check if moving forward
    if (MovementVector.Y > 0 && MovementVector.X == 0)
    {
        // set speed to 600 when moving forward
        GetCharacterMovement()->MaxWalkSpeed = 600;
    }
    else
    {
        // set speed to 450 otherwise
        GetCharacterMovement()->MaxWalkSpeed = 400;
    }
}

void AMyProjectCharacter::Look(const FInputActionValue &Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // Add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}

void AMyProjectCharacter::Jump()
{
    SetStateToUnequipped();
    if (ActionState == EActionState::Attacking) return;
    Super::Jump();
}

void AMyProjectCharacter::Equip()
{
    TArray<AWeapon *> OverlappingWeapons;

    for (AItem *Item : OverlappingItems)
    {
        AWeapon *Weapon = Cast<AWeapon>(Item);
        if (Weapon)
        {
            if (Weapon->CanBeEquipped)
            {
                OverlappingWeapons.Add(Weapon);
            }
        }
    }

    if (OverlappingWeapons.Num() == 0) return;

    // Equip new weapons
    AWeapon *OverlappingWeapon = OverlappingWeapons[0];
    UWeaponDataAsset *WeaponAsset = OverlappingWeapon->WeaponDataAsset;
    if (WeaponAsset)
    {
        AttachWeaponToHand(WeaponAsset, OverlappingWeapon);
        RemoveOverlappingItem(OverlappingWeapon);
        CurrentWeaponType = WeaponAsset->WeaponType;
        if (WeaponAsset->IsBothHandWeaponType)
        {
            // Attach one missing another hand weapon
            for (AWeapon *MissingWeapon : OverlappingWeapons)
            {
                if (MissingWeapon->WeaponDataAsset)
                {
                    UWeaponDataAsset *MissingWeaponAsset = MissingWeapon->WeaponDataAsset;
                    if (MissingWeaponAsset->WeaponType == CurrentWeaponType)
                    {
                        AttachWeaponToHand(MissingWeaponAsset, MissingWeapon);
                        RemoveOverlappingItem(MissingWeapon);
                    }
                }
            }
        }
        switch (WeaponAsset->WeaponType)
        {
        case (EWeaponType::BoxingGlove):
            CharacterState = ECharacterState::BoxingGloveEquipped;
            break;
        case (EWeaponType::LightSword):
            CharacterState = ECharacterState::LightSwordEquipped;
            break;
        case (EWeaponType::SwordAndShield):
            CharacterState = ECharacterState::SwordAndShield;
            break;
        default:
            CharacterState = ECharacterState::Unarmed;
            break;
        }
    }
}

void AMyProjectCharacter::AttachWeaponToHand(UWeaponDataAsset *WeaponAsset, AWeapon *OverlappingWeapon)
{
    if (WeaponAsset->WeaponType != CurrentWeaponType) UnequippedOldWeapon();
    GetCharacterMovement()->StopMovementImmediately();
    if (WeaponAsset->IsLeftHand)
    {
        OverlappingWeapon->Attach(GetMesh(), WeaponAsset->LeftHandSocketName, this, this);
        EquippedLeftHandWeapon = OverlappingWeapon;
        AttackMontage = EquippedLeftHandWeapon->GetWeaponAttackMontage();
        EquippedWeapons.Add(OverlappingWeapon);
    }
    else
    {
        OverlappingWeapon->Attach(GetMesh(), WeaponAsset->RightHandSocketName, this, this);
        EquippedRightHandWeapon = OverlappingWeapon;
        AttackMontage = EquippedRightHandWeapon->GetWeaponAttackMontage();
        EquippedWeapons.Add(OverlappingWeapon);
    }
    // Get weapon block montage
    UAnimMontage *WeaponBlockMontage = OverlappingWeapon->GetWeaponBlockMontage();
    if (WeaponBlockMontage) BlockMontage = WeaponBlockMontage;

    UAnimMontage *WeaponBlockSuccessMontage = OverlappingWeapon->GetWeaponBlockSuccessMontage();
    if (WeaponBlockSuccessMontage) BlockSuccessMontage = WeaponBlockSuccessMontage;

    CurrentWeaponType = WeaponAsset->WeaponType;
    IsEquippingWeapon = true;
}

void AMyProjectCharacter::Attack()
{
    SetStateToUnequipped();
    if (ActionState == EActionState::Unoccupied)
    {
        if (!IsEquippingWeapon) return;
        // Set character movement to 0 before attack
        GetCharacterMovement()->StopActiveMovement();
        PlayAttackMontage();
        ActionState = EActionState::Attacking;

        // Set IsCombatting to true
        IsCombatting = true;

        // Schedule to set back IsCombatting to false
        GetWorld()->GetTimerManager().SetTimer(CombatTimerHandle, this, &AMyProjectCharacter::SetIsCombattingToFalse,
                                               10.f, false);

        IsAttacking = true;

        // Player can not rotate while playing attack montage
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        bUseControllerRotationYaw = false;
    }
}

void AMyProjectCharacter::Block()
{
    if (EquippedWeapons.IsEmpty()) return;

    if (AnimInstance)
    {
        AnimInstance->Montage_Play(BlockMontage);
        FOnMontageBlendingOutStarted BlendingOutDelegate;
        BlendingOutDelegate.BindUObject(this, &AMyProjectCharacter::OnMontageBlendingOut);
        AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, BlockMontage);
    }
}

void AMyProjectCharacter::OnMontageBlendingOut(UAnimMontage *AnimMontage, bool bIsInterrupted)
{
    EndBlock();
}

void AMyProjectCharacter::BlockEnd()
{
    if (AnimInstance)
    {
        CanJumpToBlockEndSectionFromStartSection = false;
        FName PlayingSectionName = AnimInstance->Montage_GetCurrentSection(BlockMontage);
        if (PlayingSectionName == FName("Section1") || PlayingSectionName == FName("Section3"))
        {
            CanJumpToBlockEndSectionFromStartSection = true;
        }
        else
        {
            AnimInstance->Montage_JumpToSection("Section3", BlockMontage);
        }
    }
}

void AMyProjectCharacter::GetHit(const FVector &ImpactPoint)
{
    const FVector Forward = GetActorForwardVector();
    const FVector ImpactPointLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
    const FVector ToHitPoint = (ImpactPointLowered - GetActorLocation()).GetSafeNormal();

    float CosTheta = FVector::DotProduct(Forward, ToHitPoint);
    float Theta = FMath::Acos(CosTheta);
    Theta = FMath::RadiansToDegrees(Theta);
    const FVector CrossProduct = FVector::CrossProduct(Forward, ToHitPoint);

    if (CrossProduct.Z < 0)
    {
        Theta *= -1.f;
    }

    FName HitReactSection = "HitBodyBack";
    if (Theta < 45.f && Theta >= -45.f)
    {
        HitReactSection = "HitBodyFront";
    }
    else if (Theta >= 45.f && Theta < 135.f)
    {
        HitReactSection = "HitBodyRight";
    }
    else if (Theta < -45.f && Theta >= -135.f)
    {
        HitReactSection = "HitBodyLeft";
    }
    PlayHitReactMontage(HitReactSection);
}

void AMyProjectCharacter::PlayAttackMontage()
{
    if (AnimInstance && AttackMontage)
    {
        AnimInstance->Montage_Play(AttackMontage);
        AttackCounter = (AttackCounter % AttackMontage->GetNumSections()) + 1;
        FName SectionName = FName(*FString::Printf(TEXT("Section%d"), AttackCounter));

        // Get data of each attack section
        AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

        // Set weapon attack parameters
        TArray<UAnimMetaData *> AnimMetaData = AttackMontage->GetSectionMetaData(SectionName, true);
        if (AnimMetaData.IsEmpty()) return;
        UAttackBehavior *AttackBehavior = dynamic_cast<UAttackBehavior *>(AnimMetaData[0]);
        SetWeaponAttackBehavior(AttackBehavior->Damage, AttackBehavior->CanBeBlocked);
    }
}

void AMyProjectCharacter::AttackEnd()
{
    ActionState = EActionState::Unoccupied;
    IsAttacking = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = true;
    bUseControllerRotationYaw = true;
}

void AMyProjectCharacter::ToggleWeaponCollision(ECollisionEnabled::Type CollisionType, bool IsLeftHandWeapon)
{
    if (IsLeftHandWeapon)
    {
        if (EquippedLeftHandWeapon && EquippedLeftHandWeapon->GetWeaponHitBox())
        {
            EquippedLeftHandWeapon->IgnoreActors.Empty();
            EquippedLeftHandWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionType);
        }
    }
    else
    {
        if (EquippedRightHandWeapon && EquippedRightHandWeapon->GetWeaponHitBox())
        {
            EquippedRightHandWeapon->IgnoreActors.Empty();
            EquippedRightHandWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionType);
        }
    }
}

void AMyProjectCharacter::UnequippedOldWeapon()
{
    if (EquippedWeapons.IsEmpty()) return;
    for (AWeapon *Weapon : EquippedWeapons)
    {
        Weapon->Detach();
        Weapon = nullptr;
    }
    EquippedWeapons.Empty();
    IsEquippingWeapon = false;
}

void AMyProjectCharacter::PlayHitReactMontage(const FName &SectionName)
{
    if (AnimInstance && HitReactMontage)
    {
        AnimInstance->Montage_Play(HitReactMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AMyProjectCharacter::PlayDeathMontage()
{
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
        // Play section at random
        int8 RandomValue = FMath::RandRange(1, DeathMontage->GetNumSections());
        FName SectionName = FName();
        switch (RandomValue)
        {
        case (1):
            SectionName = FName("Death1");
            DeathPose = EDeathPose::DeathPose1;
            break;
        case (2):
            SectionName = FName("Death2");
            DeathPose = EDeathPose::DeathPose2;
            break;
        case (3):
            SectionName = FName("Death3");
            DeathPose = EDeathPose::DeathPose3;
            break;
        case (4):
            SectionName = FName("Death4");
            DeathPose = EDeathPose::DeathPose4;
            break;
        }
        AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
    }
}

float AMyProjectCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                      class AController *EventInstigator, AActor *DamageCauser)
{
    if (HealthComponent && HealthBarWidget && HealthBarWidget->HealthBar)
    {
        HealthComponent->SetCurrentHealth(DamageAmount);
        HealthBarWidget->HealthBar->SetPercent(HealthComponent->GetHealthPercent());
        if (HealthComponent->IsDead())
        {
            Die();
        }
    }
    return DamageAmount;
}

void AMyProjectCharacter::Die()
{
    ActionState = EActionState::Dead;
    PlayDeathMontage();

    // Disable input when player die
    APlayerController *PlayerController = Cast<APlayerController>(GetController());
    if (PlayerController)
    {
        DisableInput(PlayerController);
    }

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(3.f);
    // if (HealthBarWidget)
    // {
    //     HealthBarWidget->SetVisibility(false);
    // }
}

void AMyProjectCharacter::TakeDamageInterface(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                              class AController *EventInstigator, AActor *DamageCauser)
{
    TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AMyProjectCharacter::IsDead()
{
    return HealthComponent->IsDead();
}

void AMyProjectCharacter::SetStateToUnequipped()
{
    if (AnimInstance)
    {
        if (!AnimInstance->IsAnyMontagePlaying() && ActionState == EActionState::Attacking)
        {
            ActionState = EActionState::Unoccupied;
        }
    }
}

void AMyProjectCharacter::SetIsCombattingToFalse()
{
    IsCombatting = false;
}

void AMyProjectCharacter::SetWeaponAttackBehavior(float DamageAmount, bool bCanBeBlocked)
{
    if (EquippedWeapons.Num() > 0)
    {
        for (AWeapon *Weapon : EquippedWeapons)
        {
            if (Weapon)
            {
                Weapon->SetDamage(DamageAmount);
                Weapon->SetCanBlock(bCanBeBlocked);
            }
        }
    }
}

void AMyProjectCharacter::StartBlock()
{
    bIsBlocking = true;
}

void AMyProjectCharacter::EndBlock()
{
    bIsBlocking = false;
}

void AMyProjectCharacter::OnBlockSuccess()
{
    PlayAnimMontage(BlockSuccessMontage);
}