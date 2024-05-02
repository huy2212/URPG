// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BossEnemyAIC.h"
#include "Enemy/BossEnemy.h"
#include "Engine/Engine.h"

void ABossEnemyAIC::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);
    BossEnemy = Cast<ABossEnemy>(InPawn);
}

void ABossEnemyAIC::HandleSenseSight(AActor *SeenActor)
{
    Super::HandleSenseSight(SeenActor);
    if (!IsHealthBarShown)
    {
        if (BossEnemy)
        {
            BossEnemy->EnableBossHealthBarWidget();
            IsHealthBarShown = true;
        }
    }
}