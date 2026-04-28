// (c) 2025 Sarah Smith


#include "PromptData.h"

bool FPromptData::CanBeShown() const
{
    // If a prompt is SingleUse and has been selected already then we can't show it.
    if (SingleUse && HasBeenSelected) return false;

    // Otherwise, it can be shown as long as its visible.
    return Visible;
}

void FPromptData::Hide()
{
    Visible = false;
}

bool FPromptData::IsIndex(int PromptIndex, int SubIndex) const
{
    return PromptNumber == PromptIndex && SubIndex == PromptSubNumber;
}

bool FPromptData::HasEmptyText() const
{
    for (const FText& Text : PromptText)
    {
        if (Text.IsEmpty()) return true;
    }
    return false;
}

FPromptDisplayDataUnit FPromptData::GetDisplayData() const
{
    FPromptDisplayDataUnit Data;
    Data.PromptText = PromptText;
    Data.HasBeenSelected = HasBeenSelected;
    return Data;
}
