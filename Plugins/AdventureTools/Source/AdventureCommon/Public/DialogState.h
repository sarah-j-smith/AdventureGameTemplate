#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialogState.generated.h"

UENUM(BlueprintType)
enum class EDialogState: uint8
{
    /// The dialog component is not showing any information
    Hidden     = 0    UMETA(DisplayName = "Dialog is not being shown"),

    /// The UI area is showing the prompt menu for this current dialog
    /// component, but no barks for either player or NPC
    Prompts   = 1    UMETA(DisplayName = "Prompt menu is shown"),

    /// The dialog component is showing the player barks (and prompts)
    Player     = 2    UMETA(DisplayName = "Player dialog is being shown"),

    /// The dialog component is showing NPC barks (and player barks, and prompts)
    NPC        = 3    UMETA(DisplayName = "NPC dialog is being shown"),
};
