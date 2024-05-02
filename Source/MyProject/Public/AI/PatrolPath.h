

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class MYPROJECT_API APatrolPath : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APatrolPath();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    class USplineComponent *SplineComponent;

    int8 CurrentPatrolIndex = 0;

    bool IsForwardDirection = true;

    UFUNCTION(BlueprintCallable)
    FVector GetCurrentPatrolPointAsWorldPosition();

    UFUNCTION(BlueprintCallable)
    void OnPatrolPointReached();
};
