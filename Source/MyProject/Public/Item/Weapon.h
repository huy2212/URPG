// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Item/Item.h"
#include "DataAsset/WeaponDataAsset.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Interfaces/IBlockable.h"
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

    UFUNCTION(BlueprintCallable)
    void Attach(USceneComponent *InParent, FName InSocketName, AActor *NewOwner, APawn *NewInstigator);

    UFUNCTION(BlueprintCallable)
    void Detach();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UWeaponDataAsset *WeaponDataAsset;

    UPROPERTY(BlueprintReadWrite)
    bool CanBeEquipped = true;

    TArray<AActor *> IgnoreActors;

    class UBoxComponent *GetWeaponHitBox()
    {
        return this->WeaponHitBox;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UAnimMontage *GetWeaponAttackMontage()
    {
        return this->AttackMontage;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UAnimMontage *GetWeaponEquipMontage()
    {
        return this->EquipMontage;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UAnimMontage *GetWeaponUnequipMontage()
    {
        return this->UnequipMontage;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UAnimMontage *GetWeaponBlockMontage()
    {
        return this->BlockMontage;
    }

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UAnimMontage *GetWeaponBlockSuccessMontage()
    {
        return this->BlockSuccessMontage;
    }

    UFUNCTION(BlueprintCallable)
    void SetDamage(float DamageAmount)
    {
        this->Damage = DamageAmount;
    }

    UFUNCTION(BlueprintCallable)
    void SetCanBlock(bool bCanBlock)
    {
        this->CanBeBlocked = bCanBlock;
    }

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

private:
    class UBoxComponent *WeaponHitBox;

    UPROPERTY(VisibleAnywhere)
    USceneComponent *BoxTraceStart;

    UPROPERTY(VisibleAnywhere)
    USceneComponent *BoxTraceEnd;

    /**
     * Weapon anim montage
     */
    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *AttackMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *EquipMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *UnequipMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *BlockMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *BlockSuccessMontage;

    UPROPERTY(EditDefaultsOnly)
    class UParticleSystem *HitVFX;

    /**
     * Weapon attack behavior, will be set before each attack through animmetadata
     */
    UPROPERTY(EditAnywhere, category = "Weapon Properties")
    float Damage = 20.f; // Default damage value

    UPROPERTY()
    bool CanBeBlocked;
};
