// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UHitInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MYPROJECT_API IHitInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void GetHit(const FVector &ImpactPoint) = 0;

    UFUNCTION(BlueprintCallable)
    virtual void TakeDamageInterface(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                     class AController *EventInstigator, AActor *DamageCauser) = 0;

    UFUNCTION(BlueprintCallable)
    virtual bool IsDead() = 0;

    UFUNCTION(BlueprintCallable)
    virtual void Die() = 0;
};
