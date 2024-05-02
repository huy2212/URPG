// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AI/EnemyAIController.h"
#include "AI/PatrolPath.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Character/CharacterState.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Components/CapsuleComponent.h"
#include "Engine/GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"
#include "Components/HealthComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Item/Weapon.h"
#include "Engine/EngineTypes.h"
#include "Components/BoxComponent.h"
#include "BrainComponent.h"
#include "HUD/HealthBar.h"
#include "DataAsset/AttackBehavior.h"

// Sets default values
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
    // it.
    PrimaryActorTick.bCanEverTick = true;
    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetGenerateOverlapEvents(true);

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    // Initialize necessary components
    HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
    HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>("HealthBar");
    HealthBarWidget->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthBarPercent(HealthComponent->GetHealthPercent());
        HealthBarWidget->SetVisibility(false);
    }

    // Spawn weapon at start
    if (WeaponAsset)
    {
        SpawnWeapon(WeaponAsset->HolderSocketName);
    }

    AnimInstance = GetMesh()->GetAnimInstance();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CombatTarget)
    {
        const int16 DistanceToTarget = ((CombatTarget->GetActorLocation()) - GetActorLocation()).Size();
        if (DistanceToTarget > 2000)
        {
            CombatTarget = nullptr;
            if (HealthBarWidget)
            {
                HealthBarWidget->SetVisibility(false);
            }
        }
    }
}

void AEnemy::GetHit(const FVector &ImpactPoint)
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

    if (HitVFX)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, ImpactPoint, FRotator::ZeroRotator, FVector(0.3f));
    }
}

void AEnemy::PlayHitReactMontage(const FName &SectionName)
{
    if (AnimInstance && HitReactMontage)
    {
        AnimInstance->Montage_Play(HitReactMontage);
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}

void AEnemy::PlayDeathMontage()
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

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator,
                         AActor *DamageCauser)
{
    if (HealthComponent)
    {
        HealthComponent->SetCurrentHealth(DamageAmount);
        UpdateHealthBar();
        if (HealthComponent->IsDead())
        {
            Die();
        }
    }
    CombatTarget = DamageCauser;
    return DamageAmount;
}

void AEnemy::Die()
{
    ActionState = EActionState::Dead;
    AEnemyAIController *EnemyAIController = Cast<AEnemyAIController>(GetController());
    if (EnemyAIController)
    {
        EnemyAIController->SetStateToDead();
    }

    PlayDeathMontage();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(5.f);
    if (HealthBarWidget)
    {
        HealthBarWidget->SetVisibility(false);
    }
}

APatrolPath *AEnemy::GetPatrolPath()
{
    return PatrolPath;
}

void AEnemy::SetMovementType(EMovementType NewMovementType)
{
    this->MovementType = NewMovementType;
    UCharacterMovementComponent *MovementComponent = GetCharacterMovement();
    switch (NewMovementType)
    {
    case (EMovementType::Idle):
        MovementComponent->MaxWalkSpeed = 0;
        break;
    case (EMovementType::Strafe):
        MovementComponent->MaxWalkSpeed = 100;
        break;
    case (EMovementType::Walk):
        MovementComponent->MaxWalkSpeed = 150;
        break;
    case (EMovementType::Jog):
        MovementComponent->MaxWalkSpeed = 300;
        break;
    case (EMovementType::Run):
        MovementComponent->MaxWalkSpeed = 600;
        break;
    }
}

void AEnemy::SpawnWeapon(FName SocketName)
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }
    EquippedWeapon = Cast<AWeapon>(
        UGameplayStatics::BeginSpawningActorFromBlueprint(GetWorld(), BP_WeaponAsset, GetTransform(), true));
    UGameplayStatics::FinishSpawningActor(EquippedWeapon, GetTransform());
    if (EquippedWeapon)
    {
        if (WeaponAsset->IsLeftHand)
        {
            EquippedWeapon->Attach(GetMesh(), SocketName, this, this);
        }
        else
        {
            EquippedWeapon->Attach(GetMesh(), SocketName, this, this);
        }
    }
}

void AEnemy::ToggleWeaponCollision(ECollisionEnabled::Type CollisionType, bool IsLeftHandWeapon)
{
    if (IsLeftHandWeapon)
    {
        if (EquippedWeapon && EquippedWeapon->GetWeaponHitBox())
        {
            EquippedWeapon->IgnoreActors.Empty();
            EquippedWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionType);
        }
    }
    else
    {
        if (EquippedWeapon && EquippedWeapon->GetWeaponHitBox())
        {
            EquippedWeapon->IgnoreActors.Empty();
            EquippedWeapon->GetWeaponHitBox()->SetCollisionEnabled(CollisionType);
        }
    }
}

void AEnemy::SetWeaponBehavior(UAnimMontage *AttackMontage, FName SectionName)
{
    TArray<UAnimMetaData *> AnimMetaData = AttackMontage->GetSectionMetaData(SectionName, true);
    if (AnimMetaData.IsEmpty()) return;
    UAttackBehavior *AttackBehavior = dynamic_cast<UAttackBehavior *>(AnimMetaData[0]);
    SetWeaponParam(AttackBehavior->Damage, AttackBehavior->CanBeBlocked);
}

void AEnemy::SetWeaponParam(float DamageAmount, bool bCanBeBlocked)
{
    if (EquippedWeapon)
    {
        EquippedWeapon->SetDamage(DamageAmount);
        EquippedWeapon->SetCanBlock(bCanBeBlocked);
    }
}

void AEnemy::DealDamage()
{
}

void AEnemy::TakeDamageInterface(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                 class AController *EventInstigator, AActor *DamageCauser)
{
    TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AEnemy::IsDead()
{
    return HealthComponent->IsDead();
}

void AEnemy::StartBlock()
{
    bIsBlocking = true;
}

void AEnemy::EndBlock()
{
    bIsBlocking = false;
}

void AEnemy::OnBlockSuccess()
{
    BlockSuccessMontage = EquippedWeapon->GetWeaponBlockSuccessMontage();
    PlayAnimMontage(BlockSuccessMontage);
}

void AEnemy::OnMontageBlendingOut(UAnimMontage *AnimMontage, bool bIsInterrupt)
{
    EndBlock();
}

FName AEnemy::FindNextSection(UAnimMontage *AnimMontage)
{
    if (AnimInstance && AnimMontage)
    {
        AttackCounter = (AttackCounter % AnimMontage->GetNumSections()) + 1;
        FName SectionName = FName(*FString::Printf(TEXT("Section%d"), AttackCounter));

        // Set weapon attack parameters
        SetWeaponBehavior(AnimMontage, SectionName);
        return SectionName;
    }
    return "";
}

void AEnemy::SetNextWeaponRange(float &Out_AttackRange)
{
    UAnimMontage *AttackMontage = EquippedWeapon->GetWeaponAttackMontage();
    if (AttackMontage)
    {
        int8 NextAttackCounter = (AttackCounter % AttackMontage->GetNumSections()) + 1;
        FName NextSectionName = FName(*FString::Printf(TEXT("Section%d"), NextAttackCounter));

        TArray<UAnimMetaData *> AnimMetaData = AttackMontage->GetSectionMetaData(NextSectionName, true);
        if (AnimMetaData.IsEmpty()) return;
        UAttackBehavior *AttackBehavior = dynamic_cast<UAttackBehavior *>(AnimMetaData[0]);

        Out_AttackRange = AttackBehavior->Range;
    }
}

void AEnemy::SetActionState(EActionState NewActionState)
{
    ActionState = NewActionState;
}

void AEnemy::UpdateHealthBar()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthBarPercent(HealthComponent->GetHealthPercent());
        HealthBarWidget->SetVisibility(true);
    }
}