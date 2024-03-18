#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"
class UStaticMeshComponent;
class USphereComponent;

UCLASS()
class MYPROJECT_API AItem : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AItem();
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                 UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult &SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                                    UPrimitiveComponent *OtherComp, int32 OtherBodyIndex);

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent *ItemMesh;

    UPROPERTY(VisibleAnywhere)
    USphereComponent *Sphere;
};
