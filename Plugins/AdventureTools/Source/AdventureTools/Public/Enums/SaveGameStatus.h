// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "SaveGameStatus.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ESaveGameStatus : uint8
{
    /// The game save is up-to-date. No save is required.
    UpToDate   = 0    UMETA(DisplayName = "Up-to-Date"),
    
    /// The game state has changes that are not persisted to a save slot.
    Pending    = 1    UMETA(DisplayName = "Saving"),
    
    /// The save game is being async serialised. No user input is processed
    Saving     = 0    UMETA(DisplayName = "Saving"),

    /// The save game is finished serialising. User input is enabled.
    Saved      = 1    UMETA(DisplayName = "Saving"),

    /// The game is being async loaded. No user input is processed
    Loading    = 2    UMETA(DisplayName = "Loading"),
    
    /// The game is being async serialised. No user input is processed
    Loaded     = 3    UMETA(DisplayName = "Saving"),
};
