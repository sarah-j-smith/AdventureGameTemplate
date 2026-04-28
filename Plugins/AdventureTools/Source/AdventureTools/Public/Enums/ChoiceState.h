#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ChoiceState.generated.h"

UENUM(BlueprintType)
enum class EChoiceState: uint8
{
    /// The item is locked as being selected by the player and will not change
    /// when the mouse hovers other items.
    Locked     = 0    UMETA(DisplayName = "Locked"),

    /// The item is not locked as a selection. So it will change
    /// when the mouse hovers other items. 
    Unlocked   = 1    UMETA(DisplayName = "Unlocked"),
};
