// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CoreGlobals.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/SkeletalMesh.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/IBlockable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/Color.h"
#include "Engine/EngineTypes.h"
#include "Math/MathFwd.h"
#include "MyProject/MyProjectCharacter.h"
#include "UObject/ScriptInterface.h"

AWeapon::AWeapon()
{
    WeaponHitBox = CreateDefaultSubobject<UBoxComponent>(FName("WeaponHitBox"));
    WeaponHitBox->SetupAttachment(GetRootComponent());
    WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponHitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    WeaponHitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
    WeaponHitBox->SetGenerateOverlapEvents(true);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>("Box Trace Start");
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("Box Trace End");
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Attach(USceneComponent *InParent, FName InSocketName, AActor *NewOwner, APawn *NewInstigator)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
                                             EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
    ItemMesh->SetSimulatePhysics(false);
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    CanBeEquipped = false;
    WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::Detach()
{
    ItemMesh->SetSimulatePhysics(true);
    SetOwner(nullptr);
    SetInstigator(nullptr);
    CanBeEquipped = true;
    IgnoreActors.Empty();
    WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();
    TArray<AActor *> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());
    for (AActor *Actor : IgnoreActors)
    {
        ActorsToIgnore.AddUnique(Actor);
    }
    ActorsToIgnore.Add(this);
    FHitResult HitResult;
    UKismetSystemLibrary::BoxTraceSingle(this, Start, End, WeaponHitBox->GetScaledBoxExtent(), FRotator::ZeroRotator,
                                         ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::None,
                                         HitResult, true);
    AActor *HitActor = HitResult.GetActor();
    if (HitActor)
    {
        // If hit a blockable object is currently block, then ignore it
        bool IsTargetBlocking = false;
        bool DoImplementInterface = false;
        DoImplementInterface = HitActor->GetClass()->ImplementsInterface(UIBlockable::StaticClass());
        TScriptInterface<IIBlockable> BlockableInterface;
        if (DoImplementInterface)
        {
            BlockableInterface.SetInterface(Cast<IIBlockable>(HitActor));
            BlockableInterface.SetObject(HitActor);
            if (BlockableInterface)
            {
                if (BlockableInterface->IsBlocking())
                {
                    IsTargetBlocking = true;
                }
            }
        }

        // If hit a hit interface then make it take damage
        DoImplementInterface = HitActor->GetClass()->ImplementsInterface(UHitInterface::StaticClass());
        if (DoImplementInterface)
        {
            TScriptInterface<IHitInterface> HitInterface;
            HitInterface.SetInterface(Cast<IHitInterface>(HitActor));
            HitInterface.SetObject(HitActor);
            if (IsTargetBlocking && CanBeBlocked)
            {
                // if IsTargetBlocking is true, blockableinterface definitely exists
                BlockableInterface->OnBlockSuccess();
            }
            else
            {
                HitInterface->GetHit(HitResult.ImpactPoint);
                UGameplayStatics::ApplyDamage(HitActor, Damage, GetInstigator()->GetController(), this,
                                              UDamageType::StaticClass());

                // Play hit vfx
                if (HitVFX)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitVFX, HitResult.ImpactPoint,
                                                             FRotator::ZeroRotator, FVector(0.3f));
                }
            }
            IgnoreActors.AddUnique(HitActor);
        }
    }
}
