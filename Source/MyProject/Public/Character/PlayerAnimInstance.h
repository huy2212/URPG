// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterAnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UPlayerAnimInstance : public UCharacterAnimInstance
{
    GENERATED_BODY()

protected:
    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaTime) override;

    UPROPERTY(BlueprintReadOnly)
    class AMyProjectCharacter *Player;

    UPROPERTY(BlueprintReadOnly)
    ECharacterState CharacterState;
};
