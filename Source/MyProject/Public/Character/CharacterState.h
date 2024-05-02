
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
    SwordAndShield,
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
    Unoccupied,
    Passive,
    Attacking,
    Investigating,
    Strafe,
    Dead,
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
    Alive,
    DeathPose1,
    DeathPose2,
    DeathPose3,
    DeathPose4
};

UENUM(BlueprintType)
enum class EMovementType : uint8
{
    Idle,
    Strafe,
    Walk,
    Jog,
    Run,
};