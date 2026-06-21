#pragma once

#include "DoorState.h"
#include "ItemAssetType.h"
#include "NativeGameplayTags.h"

namespace AdventureGameplayTags
{
    /// Schema is:
    ///     Scope - Disposition
    ///
    ///     Door State
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Opened);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Closed);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Locked);

    ///     HotSpot values
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(HotSpot_Hidden);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(HotSpot_SpriteHidden);

    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Give);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Open);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Close);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_PickUp);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_LookAt);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_TalkTo);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Use);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Push);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(History_Triggered_Pull);
    
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Succeeded_ItemData);
    
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Treatment_Article);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Treatment_Consumable);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Treatment_Tool);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Treatment_Key);

    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Kind_Knife);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Kind_Pickle);
    ADVENTURECOMMON_API  UE_DECLARE_GAMEPLAY_TAG_EXTERN(Item_Kind_PickleKey);
    
    ADVENTURECOMMON_API extern void SetDoorState(EDoorState State, FGameplayTagContainer &Tags);

    ADVENTURECOMMON_API extern EDoorState GetDoorState(const FGameplayTagContainer &Tags);

    ADVENTURECOMMON_API extern TSet<EItemAssetType> GetItemAssetTypes(const FGameplayTagContainer &Tags);

    ADVENTURECOMMON_API extern FGameplayTagContainer HistoryGameplayTags();
};
