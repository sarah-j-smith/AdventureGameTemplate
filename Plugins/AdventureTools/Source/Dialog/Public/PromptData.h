// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Dialog.h"
#include "PromptDisplayData.h"
#include "Constants.h"

#include "PromptData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct DIALOG_API FPromptData: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    bool CanBeShown() const;

    /**
     * Mark this prompt as not visible. It will not be shown to the player. Various
     * actions or events could hide a prompt, for example if a character took an
     * action that closed off a branch of the story.
     */
    void Hide();

    bool IsIndex(int PromptIndex, int SubIndex) const;

    bool HasEmptyText() const;

    /**
     * Check if the <code>Index</code> and <code>SubIndex</code> are valid given the
     * <code>PreviousIndex</code> and <code>PreviousSubIndex</code>. If both the previous
     * values are INVALID_PROMPT_INDEX ( -1 ) then this must be the first index, and both
     * tested indices must be zero.
     * 
     * Otherwise either the Index is strictly greater than the previous, or it is the same,
     * and the SubIndex is strictly greater than the previous. If none of those three cases
     * hold true then they are invalid. An error message is logged if the pair is invalid.
     * @param PreviousIndex Index of previous row. -1 if this is the first row.
     * @param PreviousSubIndex Sub index of previous row. -1 if this is the first row.
     * @param Index Index of row to test
     * @param SubIndex SubIndex of row to test
     * @return true if the Index/SubIndex pair is valid
     */
    static bool IsValidNextIndex(int PreviousIndex, int PreviousSubIndex, int Index, int SubIndex)
    {
        if (PreviousIndex == INVALID_PROMPT_INDEX && PreviousSubIndex == INVALID_PROMPT_INDEX && Index == 0 && SubIndex == 0) return true;
        if (PreviousIndex == INVALID_PROMPT_INDEX || PreviousSubIndex == INVALID_PROMPT_INDEX)
        {
            // PreviousIndex == INVALID_PROMPT_INDEX || PreviousSubIndex == INVALID_PROMPT_INDEX means this is the first row.
            UE_LOG(LogDialog, Fatal, TEXT("Expect first row index == 0 and sub-index == 0"));
            return false;
        }
        if (PreviousIndex == Index - 1 && SubIndex == 0) return true;
        if (PreviousIndex == Index && PreviousSubIndex == SubIndex - 1) return true;
        UE_LOG(LogDialog, Display,
            TEXT("Invalid index pair: - Index: %d - SubIndex: %d vs PreviousIndex: %d - PreviousSubIndex: %d"),
            Index, SubIndex, PreviousIndex, PreviousSubIndex);
        UE_LOG(LogDialog, Display,
            TEXT("     Expected: [Index: %d - SubIndex: %d] or [Index: %d - SubIndex: %d]"),
            PreviousIndex + 1, 0, PreviousIndex, PreviousSubIndex + 1);
        return false;
    }

    int32 NextSubIndex() const
    {
        return PromptSubNumber + 1;
    }
    
    FPromptDisplayDataUnit GetDisplayData() const;

    /// Top level index for the prompt
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Indexes")
    int32 PromptNumber = 0;
    
    /// Subordinate level index for the prompt. 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Indexes")
    int32 PromptSubNumber = 0;

    /// Is the prompt visible to the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
    bool Visible = true;

    /// Is the prompt shown as selected to the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
    bool HasBeenSelected = false;

    /// Does the prompt disappear after one use. Use this with the
    /// PromptSubNumber to have new prompt replace this after its used.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility")
    bool SingleUse = false;

    /// List of texts the player will bark
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PromptText")
    TArray<FText> PromptText;

    /// List of texts the NPC will say in response
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PromptText")
    TArray<FText> NPCResponse;

    /// Table of topics to switch to
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDataTable* SwitchTopic = nullptr;

    /// Does selecting this prompt cause the conversation to finish
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool EndsConversation = false;

    /// Some event that gets fired on selection
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName Event;
};
