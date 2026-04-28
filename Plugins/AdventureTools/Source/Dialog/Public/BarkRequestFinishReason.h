// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BarkRequestFinishReason.generated.h"

UENUM(BlueprintType)
enum class EBarkRequestFinishedReason: uint8
{
    /// The bark timed out normally
    Timeout UMETA(DisplayName = "Timeout"),

    /// The bark was interrupted by the player click or tap
    Interruption UMETA(DisplayName = "Interruption"),
};