// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Character/CharacterState.h"
#include "EnemyAIInterface.generated.h"
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UEnemyAIInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MYPROJECT_API IEnemyAIInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual class APatrolPath *GetPatrolPath() = 0;

    UFUNCTION(BlueprintCallable)
    virtual void SetMovementType(EMovementType NewMovementType) = 0;

    UFUNCTION(BlueprintCallable)
    virtual void GetAttackRange(float &Out_AttackRange, float &Out_DefenseRange) = 0;

    UFUNCTION(BlueprintCallable)
    virtual void SetActionState(EActionState NewActionState) = 0;

    UFUNCTION(BlueprintCallable)
    virtual EActionState GetEnemyActionState() = 0;
};
