// (c) 2025 Sarah Smith


#include "HotSpots/Door.h"

#include "Constants.h"
#include "Gameplay/AdventureGameInstance.h"
#include "VerbType.h"

#include "AdventureGameplayTags.h"
#include "AdventureTools.h"
#include "Internationalization/StringTableRegistry.h"

bool ADoor::LockDoor()
{
    if (CanLockDoorOrItem(DoorState))
    {
        DoorState = EDoorState::Locked;
        return true;
    }
    return false;
}

bool ADoor::OpenDoor()
{
    if (CanOpenDoorOrItem(DoorState))
    {
        DoorState = EDoorState::Opened;
        return true;
    }
    return false;
}

bool ADoor::CloseDoor()
{
    if (CanCloseDoorOrItem(DoorState))
    {
        DoorState = EDoorState::Closed;
        return true;
    }
    return false;
}

EVerbType ADoor::CheckForDefaultCommand() const
{
    switch (DoorState)
    {
    case EDoorState::Closed:
        return EVerbType::Open;
    case EDoorState::Opened:
        return EVerbType::Use;
    case EDoorState::Locked:
        return EVerbType::LookAt;
    default:
        break;
    }
    return Super::CheckForDefaultCommand();
}

FGameplayTagContainer ADoor::GetTags() const
{
    FGameplayTagContainer FTags = Super::GetTags();
    if (DoorState != EDoorState::Unknown) AdventureGameplayTags::SetDoorState(DoorState, FTags);
    return FTags;
}

void ADoor::SetTags(const FGameplayTagContainer& FTags)
{
    Super::SetTags(FTags);
    DoorState = AdventureGameplayTags::GetDoorState(FTags);
}

bool ADoor::UnlockDoor()
{
    if (CanUnlockDoorOrItem(DoorState))
    {
        DoorState = EDoorState::Closed;
        return true;
    }
    return false;
}

void ADoor::OnUse_Implementation()
{
    FText DoorNameReason;
    bool CanLoad = LevelToLoad.IsValidObjectName(DoorNameReason);
#if WITH_EDITOR
    if (!CanLoad && !LevelToLoad.ToString().IsEmpty())
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("Could not use door: %s"), *DoorNameReason.ToString());
    }
#endif
    switch (DoorState)
    {
    case EDoorState::Unknown:
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "NotOpenable"));
        return;
    case EDoorState::Closed:
        Execute_OnOpen(this);
        return;
    case EDoorState::Opened:
        if (CanLoad)
        {
            UAdventureGameInstance::LoadRoom(this);
        }
        else
        {
            DoorErrors("Level to load is invalid");
        }
        return;
    case EDoorState::Locked:
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "Locked"));
        return;
    }
    BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "UseDefaultText"));
}

void ADoor::OnOpen_Implementation()
{
    switch (DoorState)
    {
    case EDoorState::Unknown:
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "NotOpenable"));
        break;
    case EDoorState::Closed:
        OpenDoor();
        break;
    case EDoorState::Opened:
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "AlreadyOpened"));
        break;
    case EDoorState::Locked:
        BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "Locked"));
        break;
    }
}

void ADoor::DoorErrors(const FString& Reason)
{
#if WITH_EDITOR
    const FText DoorName = ShortDescription.IsEmpty()
                               ? (Description.IsEmpty() ? FText::FromString("Door") : Description)
                               : ShortDescription;
    const FString Message = FString::Printf(TEXT("Door %s: %s"), *Description.ToString(), *Reason);
    GEngine->AddOnScreenDebugMessage(1, 5.0, FColor::Cyan,
                                     *Message, false, FVector2D(2.0, 2.0));
    UE_LOG(LogAdventureGame, Display, TEXT("%s"), *Message);
#endif
}
