// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "AttackBehavior.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UAttackBehavior : public UAnimMetaData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Damage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Range;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool CanBeBlocked;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool CanBeParried;
};
