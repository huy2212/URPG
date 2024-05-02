// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UHealthComponent();

    UFUNCTION(BlueprintCallable)
    void SetCurrentHealth(float SubtractAmount);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    float GetHealthPercent();

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool IsDead();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere, Category = "Health")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHealth;
};
