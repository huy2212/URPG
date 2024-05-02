// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBlockable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UIBlockable : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MYPROJECT_API IIBlockable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual void StartBlock() = 0;

    UFUNCTION(BlueprintCallable)
    virtual void EndBlock() = 0;

    UFUNCTION(BlueprintCallable)
    virtual void OnBlockSuccess() = 0;

    UFUNCTION(BlueprintCallable)
    virtual bool IsBlocking() = 0;

    UFUNCTION(BlueprintCallable)
    virtual bool CanBlock() = 0;
};
