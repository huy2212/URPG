// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"

void UHealthBarComponent::SetHealthBarPercent(float Percent) { 
    if(HealthBarWidget == nullptr)
    {
        HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
    }
    
    if(HealthBarWidget && HealthBarWidget->HealthBar)
    {
        HealthBarWidget->HealthBar->SetPercent(Percent);
    }
}
