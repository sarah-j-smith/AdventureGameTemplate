#pragma once

#include "CoreMinimal.h"

#include "PromptData.h"

#include "ConversationData.generated.h"

#define EMPTY_TEXT_ERROR TEXT("Prompt %d (row %d) has empty text")
#define SEQUENCE_ERROR TEXT("Prompt (row %d) Invalid sequence  Prev [ %d / %d ], Next [ Prompt: %d / Sub-prompt: %d ]")
#define SWITCH_AND_END_ERROR TEXT("Prompt %d (row %d) - %s switches topic but ends conversation")
#define SINGLE_USE_ERROR TEXT("Prompt %d (row %d) - should be single use, else next prompt %d (row %d) will not display.")

/**
 * A single topic loaded into memory, ready to display in game.
 */
USTRUCT(BlueprintType)
struct DIALOG_API FConversationData: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    TArray<FPromptData> ConversationPromptArray;

    /**
     * Get the prompts to display from this array. The number of prompts
     * will be n: 0 <= n <= GMax_Number_Of_Prompts. The indexes of prompts
     * will be the first ordered prompt indexes, except for SingleUse
     * prompts that have already been selected. 
     * @param OutPromptData Array to fill with prompts to display.
     */
    void DisplayPrompts(TArray<FPromptData> &OutPromptData) const;

    /**
     * Mark the prompt at the given index & sub-index as selected.
     * @param PromptIndex Prompt to mark
     * @param PromptSubIndex Prompt sub-index to markt
     */
    void MarkPromptSelected(int PromptIndex, int PromptSubIndex);

    /**
     * Calculate the number of prompts that can be shown in this topic.
     * @return Count of the number of prompts that can be shown.
     */
    int PromptsAvailableCount() const;
    
    bool Validate(FString &ErrorMessage);
    
private:
    const FPromptData *FindPromptAtIndex(int32 PromptIndex, int32 SubIndex) const;
    int GetMaxPromptIndex() const;
    
};
