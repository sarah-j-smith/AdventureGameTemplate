#pragma once

#include "VerbType.h"

class IDescribableItem;
class AHotSpot;
class UInventoryItem;

struct ADVENTURECOMMON_API FGameUtils
{
    static bool HasChangedMuch(const FVector2D& Current, const FVector2D& Previous);

    static uint32 GetUUID();

    /**
     * Find out how long to display a string for to the player.
     * @param DisplayString String to calculate the display time for
     * @return A uint32 value of the seconds to display the string for
     */
    static uint32 GetDisplayTimeForString(const FString& DisplayString);

    /**
     * Get the text as an FText for a numeric score to display in the interaction UI.
     * @param Score Numeric score value to turn into text "Score: NNN"
     * @return The resulting FText of the score
     */
    static FText GetScoreText(int32 Score);

    /**
     * For giving an item to a hotspot or item, return the text as an FText
     * translatable / i18n ready - for direct use in the UI. At least one
     * of CurrentItem or TargetItem must be non-null.
     * 
     * Uses the <code>ShortDescription</code> of the items and hotspots
     * so ensure these are correct: the default is "thing".
     *     
     * Example: "Give talisman to innkeeper" (an NPC hotspot), or "Give
     *           carrot to rabbit" (an NPC inventory item)
     * Example: "Give carrot to" - if both TargetItem and HotSpot are null
     *          for instance when targeting.
     * 
     * @param CurrentItem IDescribableItem held by the player character
     * @param TargetItem IDescribableItem  - if the target of the giving is an item
     * @param HotSpot IDescribableItem - if the target of the giving is a hotspot
     * @return FString text to display*/
    static FText GetGivingItemText(
        const IDescribableItem* CurrentItem,
        const IDescribableItem* TargetItem,
        const IDescribableItem* HotSpot
    );

    /**
     * For using an item on a hotspot or item, return the text as an FText
     * translatable / i18n ready - for direct use in the UI. 
     * 
     * Uses the <code>ShortDescription</code> of the items and hotspots
     * so ensure these are correct: the default is "thing".
     * 
     * Example: "Use pen on old notebook", "Use pen on brick wall".
     * Example: "Use pen on" - if both TargetItem and HotSpot are null
     *          for instance when targeting.
     *          
     * @param CurrentItem Item held by the player character, eg pen
     * @param TargetItem Item  - if the target of the using is an item
     * @param HotSpot HotSpot - if the target of the giving is a hotspot
     * @return FString text to display */
    static FText GetUsingItemText(
        const IDescribableItem* CurrentItem,
        const IDescribableItem* TargetItem,
        const IDescribableItem* HotSpot
    );

    /**
     * For doing a verb to an item, return the text as an FText
     * translatable / i18n ready - for direct use in the UI. CurrentItem
     * must be non-null.
     * 
     * Uses the <code>ShortDescription</code> of the item
     * so ensure this is correct: the default is "thing".
     * 
    * Example: "Open tattered notebook"
    * 
    * @param CurrentItem Item held by the player character, eg notebook
    * @param Verb EVerbType  - what action is being performed
    * @return FString text to display
    */
    static FText GetVerbWithItemText(
        const IDescribableItem* CurrentItem,
        const EVerbType Verb
    );

    /**
     * For doing a verb to an hotspot, return the text as an FText
     * translatable / i18n ready - for direct use in the UI. Hotspot
     * must be non-null.
     * 
     * Uses the <code>ShortDescription</code> of the hotspot
     * so ensure this is correct: the default is "thing".
     * 
    * Example: "Open door"
    * @param HotSpot HotSpot targeted by the player character, eg door
    * @param Verb EVerbType  - what action is being performed
    * @return FText text to display
    */
    static FText GetVerbWithHotSpotText(
        const IDescribableItem* HotSpot,
        const EVerbType Verb
    );

    /**
     * Create an array of texts, by splitting the given text on the
     * newline character "\n"
     * @param NewText Text to split
     * @return Array of texts.
     */
    static TArray<FText> NewLineSeparatedToArrayText(const FText& NewText);
    
    /**
     * Create an array of strings, by splitting the given text on the
     * newline character "\n"
     * @param NewString Text to split
     * @return Array of strings.
     */
    static TArray<FString> NewLineSeparatedToArrayString(const FString& NewString);

    /**
     * Wrap a line of text so that it fits into the given length.
     * @param NewText Text to wrap
     * @param MaxLength Maximum length of a line
     * @return Array of lines, all less than or equal to MaxLength
     */
    static TArray<FText> WrapTextLinesToMaxCharacters(const FText& NewText, const int32 MaxLength = 30);
};
