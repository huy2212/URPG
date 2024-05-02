// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CharacterAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "Enemy/Enemy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "MyProject/MyProjectCharacter.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<ACharacter>(TryGetPawnOwner());
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
        Acceleration = UKismetMathLibrary::VSizeXY(CharacterMovement->GetCurrentAcceleration());
        if (Speed > 3.f && Acceleration != 0)
        {
            ShouldMove = true;
        }
        else
        {
            ShouldMove = false;
        }

        IsFalling = CharacterMovement->IsFalling();

        Direction = UKismetAnimationLibrary::CalculateDirection(CharacterMovement->Velocity,
                                                                GetOwningActor()->GetActorRotation());
        if (ShouldMove)
        {
            LastDirection = Direction;
        }
    }
}