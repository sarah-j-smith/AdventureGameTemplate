#include "Items/ItemDataList.h"

UItemDataAsset* FItemDataList::GetItemDataAssetForAction(const EVerbType Verb) const
{
    for (const FItemDataWrapper &Wrapper : ItemDataRecords)
    {
        if (Wrapper.ActiveVerb == Verb) return Wrapper.UnwrapItemDataAsset();
    }
    return nullptr;
}