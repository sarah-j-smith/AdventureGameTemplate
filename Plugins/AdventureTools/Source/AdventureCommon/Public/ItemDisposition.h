// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "ItemDisposition.generated.h"

/**
 * What is currently happening with a particular item? 
 */
UENUM(BlueprintType)
enum class EItemDisposition: uint8
{
    /// State of the item is not known
    Unknown     = 0    UMETA(DisplayName = "Unknown"),
    
    /// The item was added to the inventory
    Added      = 1    UMETA(DisplayName = "Added"),

    /// The item was removed from the inventory
    Removed     = 2    UMETA(DisplayName = "Removed"),
    
    /// The inventory was reloaded, eg during a game save or load
    Reloaded    = 3    UMETA(DisplayName = "Reloaded"),
    
    /// An error ocurred with the given item - it couldn't be loaded
    /// see the logs for specific details
    Error       = 4    UMETA(DisplayName = "Error"),
};