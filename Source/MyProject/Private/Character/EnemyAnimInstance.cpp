// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Enemy = Cast<AEnemy>(Super::Character);
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (Enemy)
    {
        EnemyActionState = Enemy->GetActionState();
    }
}