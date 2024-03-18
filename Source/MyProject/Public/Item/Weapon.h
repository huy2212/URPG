// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Weapon.generated.h"
/**
 *
 */
UCLASS()
class MYPROJECT_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon();
    void Attach(USceneComponent *InParent, FName InSocketName);

    void Detach();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UWeaponDataAsset *WeaponDataAsset;

    UPROPERTY(VisibleAnywhere)
    class UBoxComponent *WeaponHitBox;

protected:
    virtual void BeginPlay() override;
    virtual void OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                 UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult &SweepResult) override;
    virtual void OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex) override;

    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UPROPERTY(EditDefaultsOnly)
    class UPhysicalMaterial *WeaponPhysicMaterial;

private:
    UPROPERTY(VisibleAnywhere)
    USceneComponent *BoxTraceStart;

    UPROPERTY(VisibleAnywhere)
    USceneComponent *BoxTraceEnd;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *AttackMontage;

public:
    UBoxComponent *GetWeaponHitBox()
    {
        return WeaponHitBox;
    }

    UAnimMontage *GetWeaponAttackMontage()
    {
        return AttackMontage;
    }
};
