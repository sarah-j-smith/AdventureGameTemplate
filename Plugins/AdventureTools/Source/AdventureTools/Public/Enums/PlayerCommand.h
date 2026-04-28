#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerCommand.generated.h"

/// What commands has the player current issued in the UX.
/// Note that sometimes the game will default the CurrentAction if the player
/// has not specifically clicked on a command verb, eg WalkTo if over the game
/// window, and LookAt if over the inventory. These states are for when the player
/// has specifically given a command.
UENUM(BlueprintType)
enum class EPlayerCommand : uint8
{
    /// The player has not clicked or hovered to activate any command
    None = 0           UMETA(DisplayName = "NONE"),

    /// The player is hovering the UI
    Hover = 1          UMETA(DisplayName = "Hover"),

    /// The player has chosen a command on a verb - other than Use or Give
    /// and now must select a hotspot or item target for that verb.
    VerbPending = 2    UMETA(DisplayName = "Verb Pending"),

    /// The player has chosen a command by clicking on Use. If they click a
    /// HotSpot it will be Active "used" - eg "Use lever". If they click an item,
    /// we go to <code>Targeting</code> to select the HotSpot or other Item
    /// which will be the target of the "Use On".
    UsePending = 3     UMETA(DisplayName = "Use Pending"),

    /// The player has chosen a command by clicking on Give. If they click an item,
    /// we go to <code>Targeting</code> to select the HotSpot or other Item
    /// which will be the target of the "Give to".
    GivePending = 4    UMETA(DisplayName = "Give Pending"),

    /// The player has clicked on an Item as a <code>Source</code> for Use or Give.
    /// Now we are waiting for the player to pick a another item or hotspot
    /// for the Use or Give which will become the <code>Target</code>.
    Targeting = 5      UMETA(DisplayName = "Targeting"),

    /// A verb is being actioned
    Active = 6         UMETA(DisplayName = "Active"),
	
    /// A Walk to or Look at was activated via a single click
    InstantActive = 7  UMETA(DisplayName = "Instant Active"),

    /// An action finalised and now the player is showing some text as feedback
    /// that must finish displaying before the actions are stopped.
};
