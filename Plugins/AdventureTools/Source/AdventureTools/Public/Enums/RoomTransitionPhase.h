#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RoomTransitionPhase.generated.h"

UENUM(BlueprintType)
enum class ERoomTransitionPhase: uint8
{
    /// The game is just launching and the player has not issued any commands yet
    GameNotStarted UMETA(DisplayName = "GameNotStarted"),

    /// The game is loading up the starting room, and play is locked
    LoadStartingRoom UMETA(DisplayName = "LoadStartingRoom"),

    /// The game has been played for a time, and a valid loaded room is currently shown
    RoomCurrent UMETA(DisplayName = "RoomCurrent"),

    /// Yet another new room is being loaded, the game is locked.
    LoadNewRoom UMETA(DisplayName = "LoadNewRoom"),

    /// The room that was loading is now loaded, and is about to be set up
    NewRoomLoaded UMETA(DisplayName = "NewRoomLoaded"),

    /// An old room that was in memory is being unloaded
    UnloadOldRoom UMETA(DisplayName = "UnloadOldRoom"),

    /// The room that was unloading has finished being removed from memory
    RoomUnloaded UMETA(DisplayName = "RoomUnloaded"),

    /// Everything is nearly done, there's a short delay being processed to allow settling
    DelayProcessing UMETA(DisplayName = "DelayProcessing"),
};
