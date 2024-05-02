// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/EnemyAIController.h"
#include "BossEnemyAIC.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API ABossEnemyAIC : public AEnemyAIController
{
    GENERATED_BODY()

protected:
    virtual void HandleSenseSight(AActor *SeenActor) override;

    virtual void OnPossess(APawn *InPawn) override;

private:
    bool IsHealthBarShown = false;

    class ABossEnemy *BossEnemy;
};
