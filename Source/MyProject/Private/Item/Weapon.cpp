// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/SkeletalMesh.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"

AWeapon::AWeapon()
{
    WeaponHitBox = CreateDefaultSubobject<UBoxComponent>(FName("WeaponHitBox"));
    WeaponHitBox->SetupAttachment(GetRootComponent());
    WeaponHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponHitBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponHitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    WeaponHitBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>("Box Trace Start");
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>("Box Trace End");
    BoxTraceEnd->SetupAttachment(GetRootComponent());
    ItemMesh->BodyInstance.SetPhysMaterialOverride(WeaponPhysicMaterial);
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    WeaponHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                              UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult &SweepResult)
{
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                 UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::Attach(USceneComponent *InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,
                                             EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
    ItemMesh->SetSimulatePhysics(false);
}

void AWeapon::Detach()
{
    FDetachmentTransformRules DetachmentRules(EDetachmentRule::KeepRelative, false);
    // ItemMesh->DetachFromComponent(DetachmentRules);
    ItemMesh->SetSimulatePhysics(true);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();
    const FVector End = BoxTraceEnd->GetComponentLocation();
    TArray<AActor *> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    FHitResult HitResult;
    UKismetSystemLibrary::BoxTraceSingle(this, Start, End, FVector(5, 5, 5), BoxTraceStart->GetComponentRotation(),
                                         ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
                                         EDrawDebugTrace::ForDuration, HitResult, true);
}
