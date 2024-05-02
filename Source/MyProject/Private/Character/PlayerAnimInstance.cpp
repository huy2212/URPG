// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PlayerAnimInstance.h"
#include "MyProject/MyProjectCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Player = Cast<AMyProjectCharacter>(Super::Character);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (Player)
    {
        CharacterState = Player->GetCharacterState();
    }
}