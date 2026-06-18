#include "ItemDataList.h"

#include "StoryAction.h"

UStoryAction* FItemDataList::GetItemDataAssetForAction(const EVerbType Verb) const
{
    for (const FItemDataWrapper &Wrapper : ItemDataRecords)
    {
        if (Wrapper.ActiveVerb == Verb) return Wrapper.UnwrapItemDataAsset();
    }
    return nullptr;
}
