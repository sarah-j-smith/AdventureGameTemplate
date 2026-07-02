#include "AdventureGameplayTags.h"

#include "ItemAssetType.h"

namespace AdventureGameplayTags
{
    // If none of these tags is present for an item or hotspot then it's door state is "Unknown"
    // and cannot be locked, unlocked, opened, closed or walked through.
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Opened, "State.Opened", "The door or item is opened and accessible");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Closed, "State.Closed", "The door or item is closed and unlocked");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Locked, "State.Locked", "The door or item is closed and locked");

    // Status for various items in the objects that appear in the game. The default state is when the
    // tag is not present. For example if its not Hidden then its Visible.
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(HotSpot_Hidden, "HotSpot.Hidden", "The entire HotSpot actor is hidden");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(HotSpot_SpriteHidden, "HotSpot.SpriteHidden", "The PickUp sprite is hidden");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_ItemData_Succeeded, "History.ItemData.Succeeded", "The item data success case was triggered");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_ItemData_Failed, "History.ItemData.Succeeded", "The item data success case was triggered");
    
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Give, "History.Triggered.Give", "The item or hotspot had the give verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Open, "History.Triggered.Open", "The item or hotspot had the open verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Close, "History.Triggered.Close", "The item or hotspot had the close verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_PickUp, "History.Triggered.PickUp", "The item or hotspot had the pick-up verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_LookAt, "History.Triggered.LookAt", "The item or hotspot had the look-at verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_TalkTo, "History.Triggered.TalkTo", "The item or hotspot had the talk-to verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Use, "History.Triggered.Use", "The item or hotspot had the use verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Push, "History.Triggered.Push", "The item or hotspot had the push verb used on it");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(History_Triggered_Pull, "History.Triggered.Pull", "The item or hotspot had the pull verb used on it");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Fail, "Item.Treatment.Fail", "This story item will always fail");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Succeed, "Item.Treatment.Succeed", "This story item will always succeed");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Article, "Item.Treatment.Article", "Object that can be held, examined and given");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Consumable, "Item.Treatment.Consumable", "After being successfully used it is destroyed");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Tool, "Item.Treatment.Tool", "Can be used on another item to create a brand new item");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Treatment_Key, "Item.Treatment.Key", "Can be used on a hotspot to lock or unlock it");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Kind_Knife, "Item.Kind.Knife", "A sharp knife (Example item)");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Kind_Pickle, "Item.Kind.Pickle", "Tasty pickled cucumber (Example item)");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(Item_Kind_PickleKey, "Item.Kind.PickleKey", "Key made from a pickle (Example item)");
    
    FGameplayTagContainer HistoryGameplayTags()
    {
        static FGameplayTagContainer Tags;
        if (Tags.Num() > 0) return Tags;
        FGameplayTagContainer HistoryTags;
        HistoryTags.AddTagFast(History_Triggered_Give);
        HistoryTags.AddTagFast(History_Triggered_Open);
        HistoryTags.AddTagFast(History_Triggered_Close);
        HistoryTags.AddTagFast(History_Triggered_PickUp);
        HistoryTags.AddTagFast(History_Triggered_LookAt);
        HistoryTags.AddTagFast(History_Triggered_TalkTo);
        HistoryTags.AddTagFast(History_Triggered_Use);
        HistoryTags.AddTagFast(History_Triggered_Push);
        HistoryTags.AddTagFast(History_Triggered_Pull);
        Tags.AppendTags(HistoryTags);
        return HistoryTags;
    };
    
    void SetDoorState(EDoorState State, FGameplayTagContainer &Tags)
    {
        Tags.RemoveTag(AdventureGameplayTags::State_Closed);
        Tags.RemoveTag(AdventureGameplayTags::State_Opened);
        Tags.RemoveTag(AdventureGameplayTags::State_Locked);
        switch (State)
        {
        case EDoorState::Unknown:
            break;
        case EDoorState::Closed:
            Tags.AddTag(AdventureGameplayTags::State_Closed);
            break;
        case EDoorState::Opened:
            Tags.AddTag(AdventureGameplayTags::State_Opened);
            break;
        case EDoorState::Locked:
            Tags.AddTag(AdventureGameplayTags::State_Locked);
            break;
        }
    }

    TSet<EItemAssetType> GetItemAssetTypes(const FGameplayTagContainer &Tags)
    {
        TSet<EItemAssetType> Result;
        if (Tags.HasTag(Item_Treatment_Article)) Result.Add(EItemAssetType::Article);
        if (Tags.HasTag(Item_Treatment_Consumable)) Result.Add(EItemAssetType::Consumable);
        if (Tags.HasTag(Item_Treatment_Tool)) Result.Add(EItemAssetType::Tool);
        if (Tags.HasTag(Item_Treatment_Key)) Result.Add(EItemAssetType::Key);
        return Result;
    }

    EDoorState GetDoorState(const FGameplayTagContainer &Tags)
    {
        if (Tags.HasTag(AdventureGameplayTags::State_Closed)) return EDoorState::Closed;
        if (Tags.HasTag(AdventureGameplayTags::State_Opened)) return EDoorState::Opened;
        if (Tags.HasTag(AdventureGameplayTags::State_Locked)) return EDoorState::Locked;
        return EDoorState::Unknown;
    }
};