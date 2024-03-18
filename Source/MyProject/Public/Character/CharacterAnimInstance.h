// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterState.h"
#include "CharacterAnimInstance.generated.h"

/**
 *
 */
UCLASS() class MYPROJECT_API UCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class AMyProjectCharacter *Character;

    UPROPERTY(BlueprintReadOnly)
    class UCharacterMovementComponent *CharacterMovement;

    UPROPERTY(BlueprintReadOnly)
    float Speed;

    UPROPERTY(BlueprintReadOnly)
    bool IsFalling;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    ECharacterState CharacterState;
};
