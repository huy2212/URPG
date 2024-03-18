#include "Item/Item.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AItem::AItem()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    RootComponent = ItemMesh;

    Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    Sphere->SetupAttachment(ItemMesh);
    Sphere->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
#include "MyProject/MyProjectCharacter.h" // Add this include statement

void AItem::BeginPlay()
{
    Super::BeginPlay();
    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItem::OnSphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                            UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                            const FHitResult &SweepResult)
{
    AMyProjectCharacter *MyProjectCharacter = Cast<AMyProjectCharacter>(OtherActor);
    if (MyProjectCharacter)
    {
        MyProjectCharacter->SetOverlappingItem(this);
    }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
                               UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    AMyProjectCharacter *MyProjectCharacter = Cast<AMyProjectCharacter>(OtherActor);
    if (MyProjectCharacter)
    {
        MyProjectCharacter->SetOverlappingItem(nullptr);
    }
}
