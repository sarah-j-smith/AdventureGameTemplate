// (c) 2025 Sarah Smith


#include "ItemDataWrapper.h"
#include "StoryAction.h"
#include "AdventureCommon.h"


UStoryAction *FItemDataWrapper::UnwrapItemDataAsset() const
{
    if (UStoryAction *UnwrappedItemDataAsset = ItemDataAsset.LoadSynchronous())
        return UnwrappedItemDataAsset;
    UE_LOG(LogAdventureCommon, Warning, TEXT("ItemDataAsset for %s is not loaded"), *ItemDataTitle);
    return nullptr;
}
