// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/BossEnemy.h"
#include "Components/HealthComponent.h"
#include "HUD/HealthBar.h"

void ABossEnemy::EnableBossHealthBarWidget()
{
    if (WidgetClass)
    {
        BossHealthBarWidget = Cast<UHealthBar>(CreateWidget(GetWorld(), WidgetClass));
        if (BossHealthBarWidget)
        {
            BossHealthBarWidget->AddToPlayerScreen();
        }
    }
}

void ABossEnemy::UpdateHealthBar()
{
    if (HealthComponent)
    {
        BossHealthBarWidget->HealthBar->SetPercent(HealthComponent->GetHealthPercent());
    }
}