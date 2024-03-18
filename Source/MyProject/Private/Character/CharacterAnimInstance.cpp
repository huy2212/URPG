// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyProject/MyProjectCharacter.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<AMyProjectCharacter>(TryGetPawnOwner());
    if (Character)
    {
        CharacterMovement = Character->GetCharacterMovement();
    }
}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if (CharacterMovement)
    {
        Speed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
        IsFalling = CharacterMovement->IsFalling();
        CharacterState = Character->GetCharacterState();
    }
}