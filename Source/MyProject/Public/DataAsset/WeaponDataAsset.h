

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    Unequipped,
    BoxingGlove,
    LightSword,
    Pistol,
    SwordAndShield,
    TwoHandSword,
};
/**
 *
 */
UCLASS(BlueprintType)
class MYPROJECT_API UWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftHandSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightHandSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName HolderSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool IsLeftHand;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool IsBothHandWeaponType;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool CanBlock;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EWeaponType WeaponType;
};
