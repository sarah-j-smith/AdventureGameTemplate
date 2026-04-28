#pragma once

#include "CoreMinimal.h"

#include "DoorState.generated.h"

UENUM(BlueprintType)
enum class EDoorState: uint8
{
    /// State of door is not known. 
    Unknown     = 0    UMETA(DisplayName = "Unknown"),
    
    /// Player cannot pass through this door or item, but can open it
    Closed      = 1    UMETA(DisplayName = "Closed"),

    /// Player can pass through this door or access this item
    Opened     = 2    UMETA(DisplayName = "Opened"),

    /// Player cannot open this door or item
    Locked     = 3    UMETA(DisplayName = "Locked"),
};

ADVENTURECOMMON_API bool CanLockDoorOrItem(EDoorState CurrentDoorState);

ADVENTURECOMMON_API bool CanUnlockDoorOrItem(EDoorState CurrentDoorState);

ADVENTURECOMMON_API bool CanOpenDoorOrItem(EDoorState CurrentDoorState);

ADVENTURECOMMON_API bool CanCloseDoorOrItem(EDoorState CurrentDoorState);