// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UEnemyAnimInstance : public UCharacterAnimInstance
{
    GENERATED_BODY()

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AEnemy *Enemy;

    UPROPERTY(BlueprintReadOnly)
    EActionState EnemyActionState;
};
