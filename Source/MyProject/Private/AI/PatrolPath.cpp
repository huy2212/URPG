// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/PatrolPath.h"
#include "Components/SplineComponent.h"
#include "Math/MathFwd.h"

// Sets default values
APatrolPath::APatrolPath()
{
    PrimaryActorTick.bCanEverTick = false;

    SplineComponent = CreateDefaultSubobject<USplineComponent>("Spline");
    SplineComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APatrolPath::BeginPlay()
{
    Super::BeginPlay();
}

void APatrolPath::OnPatrolPointReached()
{
    if (IsForwardDirection)
    {
        CurrentPatrolIndex += 1;
        if (CurrentPatrolIndex == SplineComponent->GetNumberOfSplinePoints() - 1)
        {
            IsForwardDirection = false;
        }
    }
    else
    {
        CurrentPatrolIndex -= 1;
        if (CurrentPatrolIndex == 0)
        {
            IsForwardDirection = true;
        }
    }
}

FVector APatrolPath::GetCurrentPatrolPointAsWorldPosition()
{
    return SplineComponent->GetWorldLocationAtSplinePoint(CurrentPatrolIndex);
}
