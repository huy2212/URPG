// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "BossEnemy.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API ABossEnemy : public AEnemy
{
    GENERATED_BODY()

public:
    void EnableBossHealthBarWidget();

protected:
    virtual void UpdateHealthBar() override;

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> WidgetClass;

    UPROPERTY(VisibleAnywhere)
    class UHealthBar *BossHealthBarWidget;
};
