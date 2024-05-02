#include "Item/Item.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MyProject/MyProjectCharacter.h"

AItem::AItem()
{
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;

    // Set up item sphere overlap
    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    Sphere->SetupAttachment(GetRootComponent());
    Sphere->SetGenerateOverlapEvents(true);
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
}

void AItem::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                            UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult &SweepResult)
{
    AMyProjectCharacter *MyProjectCharacter = Cast<AMyProjectCharacter>(OtherActor);
    if (MyProjectCharacter)
    {
        MyProjectCharacter->AddOverlappingItem(this);
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                               UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    AMyProjectCharacter *MyProjectCharacter = Cast<AMyProjectCharacter>(OtherActor);
    if (MyProjectCharacter)
    {
        MyProjectCharacter->RemoveOverlappingItem(this);
    }
}
