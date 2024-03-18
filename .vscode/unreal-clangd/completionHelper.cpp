/*
*  Completion helper to help load Unreal Engine symbols into clangd auto completion
*  If we didn't do it this way, we would need to polute the project's compile_commands.json
*
*  You need to edit this to include any plugin headers or other important headers missing
*
*  I'll update this based on user recommendations
*/

// Warning: Intellisense use only. Do not include these next two includes in your own code!
#include "Engine.h"
#include "Runtime/Engine/Public/EngineSharedPCH.h"

// Kismet Math
#include "Kismet/KismetMathLibrary.h"

// For Unreal Multiplayer
#include "Net/UnrealNetwork.h"
