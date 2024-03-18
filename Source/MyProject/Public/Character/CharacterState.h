
#pragma once

#include "HAL/Platform.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Unarmed,
    Injured,
    BoxingGloveEquipped,
    LightSwordEquipped,
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
    Unoccupied,
    Attacking
};