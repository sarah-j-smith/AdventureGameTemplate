// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BarkAction.generated.h"

UENUM(BlueprintType)
enum class EBarkAction: uint8
{
    /// The HUD is not showing any barks and none are scheduled
    NotBarking UMETA(DisplayName = "Not Barking"),

    /// The HUD is not showing any barks and none are scheduled
    PlayerBarking UMETA(DisplayName = "Player Barking"),
    
    /// The HUD is not showing any barks and none are scheduled
    NPCBarking UMETA(DisplayName = "NPC Barking"),
};