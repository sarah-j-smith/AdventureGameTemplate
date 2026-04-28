// (c) 2025 Sarah Smith


#include "Items/ItemDataWrapper.h"

#include "Items/ItemDataAsset.h"
#include "AdventureTools.h"

UItemDataAsset *FItemDataWrapper::UnwrapItemDataAsset() const
{
    if (UItemDataAsset *UnwrappedItemDataAsset = ItemDataAsset.LoadSynchronous())
        return UnwrappedItemDataAsset;
    UE_LOG(LogAdventureGame, Warning, TEXT("ItemDataAsset for %s is not loaded"), *ItemDataTitle);
    return nullptr;
}
