#include "DoorState.h"

#include "AdventureCommon.h"

bool CanLockDoorOrItem(const EDoorState CurrentDoorState)
{
    if (CurrentDoorState == EDoorState::Closed) return true;
#if WITH_EDITOR
    switch(CurrentDoorState)
    {
    case EDoorState::Unknown:
        UE_LOG(LogAdventureCommon, Error, TEXT("Locking unlockable door"));
        break;
    case EDoorState::Opened:
        UE_LOG(LogAdventureCommon, Error, TEXT("Locking opened door"));
        break;
    case EDoorState::Locked:
        UE_LOG(LogAdventureCommon, Error, TEXT("Locking already locked door"));
        break;
    default:
        break;
    }
#endif
    return false;
}

bool CanUnlockDoorOrItem(const EDoorState CurrentDoorState)
{
    if (CurrentDoorState == EDoorState::Locked) return true;
#if WITH_EDITOR
    switch(CurrentDoorState)
    {
    case EDoorState::Unknown:
        UE_LOG(LogAdventureCommon, Error, TEXT("Unlocking unlockable door - does not support being locked"));
        break;
    case EDoorState::Opened:
        UE_LOG(LogAdventureCommon, Display, TEXT("Unlocking opened door"));
        break;
    case EDoorState::Closed:
        UE_LOG(LogAdventureCommon, Log, TEXT("Unlocking already unlocked, closed door"));
        break;
    default:
        break;
    }
#endif
    return false;
}

bool CanOpenDoorOrItem(const EDoorState CurrentDoorState)
{
    if (CurrentDoorState == EDoorState::Closed) return true;
#if WITH_EDITOR
    switch(CurrentDoorState)
    {
    case EDoorState::Unknown:
        UE_LOG(LogAdventureCommon, Error, TEXT("Opening unknown door state - this does not support being opened/closed"));
        break;
    case EDoorState::Opened:
        UE_LOG(LogAdventureCommon, Display, TEXT("Opening opened door"));
        break;
    case EDoorState::Locked:
        UE_LOG(LogAdventureCommon, Log, TEXT("Opening locked, closed door - unlock first"));
        break;
    default:
        break;
    }
#endif
    return false;
}

bool CanCloseDoorOrItem(const EDoorState CurrentDoorState)
{
    if (CurrentDoorState == EDoorState::Opened) return true;
#if WITH_EDITOR
    switch(CurrentDoorState)
    {
    case EDoorState::Unknown:
        UE_LOG(LogAdventureCommon, Error, TEXT("Closing unopenable door - this does not support being opened/closed"));
        break;
    case EDoorState::Locked:
        UE_LOG(LogAdventureCommon, Log, TEXT("Closing locked door"));
        break;
    case EDoorState::Closed:
        UE_LOG(LogAdventureCommon, Log, TEXT("Closing already closed door"));
        break;
    default:
        break;
    }
#endif
    return false;
}
