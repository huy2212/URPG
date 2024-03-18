

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unequipped,
    BoxingGlove,
    LightSword
};

/**
 *
 */
UCLASS(BlueprintType)
class MYPROJECT_API UWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName LeftHandSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RightHandSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsRightHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;
};
