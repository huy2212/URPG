// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void UHealthComponent::SetCurrentHealth(float SubtractAmount)
{
    CurrentHealth = FMath::Clamp(CurrentHealth - SubtractAmount, 0, MaxHealth);
}

float UHealthComponent::GetHealthPercent()
{
    return CurrentHealth / MaxHealth;
}

bool UHealthComponent::IsDead()
{
    return CurrentHealth == 0;
}