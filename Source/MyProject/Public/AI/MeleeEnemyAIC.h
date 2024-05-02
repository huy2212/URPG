// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/EnemyAIController.h"
#include "MeleeEnemyAIC.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API AMeleeEnemyAIC : public AEnemyAIController
{
    GENERATED_BODY()

private:
    class AEnemy *Enemy;
};
