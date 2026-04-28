#include "ConversationData.h"

void FConversationData::DisplayPrompts(TArray<FPromptData>& OutPromptData) const
{
    OutPromptData.Empty();
    const int MaxPromptIndex = GetMaxPromptIndex();
    int PromptIndex = 0;
    int PromptSubindex = 0;
    int PromptCount = 0;
    while (PromptCount < GMax_Number_Of_Prompts && PromptIndex <= MaxPromptIndex)
    {
        if (const FPromptData* Prompt = FindPromptAtIndex(PromptIndex, PromptSubindex))
        {
            while (Prompt && Prompt->SingleUse && Prompt->HasBeenSelected)
            {
                Prompt = FindPromptAtIndex(PromptIndex, ++PromptSubindex);
            }
            if (Prompt)
            {
                OutPromptData.Add(*Prompt);
                ++PromptCount;
            }
        }
        PromptIndex++;
        PromptSubindex = 0;
    }
    if (OutPromptData.Num() == 0)
    {
        UE_LOG(LogDialog, Warning, TEXT("No prompts selected."));
    }
}

void FConversationData::MarkPromptSelected(int PromptIndex, int SubIndex)
{
    FPromptData* FoundResult = ConversationPromptArray.FindByPredicate(
        [PromptIndex, SubIndex](const FPromptData& PromptData)
        {
            return PromptData.IsIndex(PromptIndex, SubIndex);
        });
    FoundResult->HasBeenSelected = true;
    if (FoundResult->SingleUse)
    {
        FoundResult->Visible = false;
    }
}

int FConversationData::PromptsAvailableCount() const
{
    int Count = 0;
    TSet<int32> PromptIndexes;
    for (FPromptData Prompt : ConversationPromptArray)
    {
        if (Prompt.CanBeShown() && !PromptIndexes.Contains(Prompt.PromptNumber))
        {
            PromptIndexes.Add(Prompt.PromptNumber);
            ++Count;
        }
    }
    return Count;
}

bool FConversationData::Validate(FString &ErrorMessage)
{
    int32 PreviousPromptIndex = -1;
    int32 PreviousPromptSubindex = -1;
    bool PreviousRowSingleUse = false;
    int row = 0;
    for (const FPromptData& Prompt : ConversationPromptArray)
    {
        if (Prompt.PromptText.IsEmpty() || Prompt.HasEmptyText())
        {
            ErrorMessage = FString::Printf(EMPTY_TEXT_ERROR, row, Prompt.PromptNumber);
            return false;
        }
        if (Prompt.EndsConversation && Prompt.SwitchTopic)
        {
            ErrorMessage = FString::Printf(SWITCH_AND_END_ERROR,
                Prompt.PromptNumber, row, *(Prompt.PromptText[0].ToString()));
            return false;
        }
        if (!FPromptData::IsValidNextIndex(PreviousPromptIndex, PreviousPromptSubindex,
            Prompt.PromptNumber, Prompt.PromptSubNumber))
        {
            ErrorMessage = FString::Printf(
                SEQUENCE_ERROR,
                row, PreviousPromptIndex, PreviousPromptSubindex, Prompt.PromptNumber, Prompt.PromptSubNumber);
            return false;
        }
        if (Prompt.PromptNumber == PreviousPromptIndex && !PreviousRowSingleUse)
        {
            ErrorMessage = FString::Printf(SINGLE_USE_ERROR, PreviousPromptIndex, row - 1, Prompt.PromptNumber, row);
            return false;
        }
        PreviousPromptIndex = Prompt.PromptNumber;
        PreviousPromptSubindex = Prompt.PromptSubNumber;
        PreviousRowSingleUse = Prompt.SingleUse;
        row += 1;
    }
    return true;
}

const FPromptData* FConversationData::FindPromptAtIndex(int32 PromptIndex, int32 SubIndex) const
{
    return ConversationPromptArray.FindByPredicate(
        [PromptIndex, SubIndex](const FPromptData& PromptData)
        {
            return PromptData.PromptNumber == PromptIndex && PromptData.PromptSubNumber == SubIndex;
        });
}

int FConversationData::GetMaxPromptIndex() const
{
    int MaxPromptIndex = -1;
    for (FPromptData PromptData : ConversationPromptArray)
    {
        MaxPromptIndex = std::max(PromptData.PromptNumber, MaxPromptIndex);
    }
    return MaxPromptIndex;
}
