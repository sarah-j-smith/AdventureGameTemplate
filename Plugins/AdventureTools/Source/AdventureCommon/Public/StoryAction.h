// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "StoryAction.generated.h"

/**
 * Data and functions to handle items interacting with hotspots or other
 * items, in a re-usable way. Placing the functionality here avoids
 * duplicating it in each item that needs to have the behavior.
 *
 * Can create keys that unlock doors, knives that cut objects and so on,
 * without having to write huge blueprints for common use cases.
 */
UCLASS()
class ADVENTURECOMMON_API UStoryAction : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	/// Set to true to indicate no more processing should be done on this story
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	bool bHandled = false;
	
	/// Score related to activating this item
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Score")
	int32 ScoreOnSuccess;

	/// Has the Success case been triggered before? Any other custom tags
	/// needed for blueprint scripting with this item data. Generally leave this as _NONE_
	/// unless you want to disable the action by marking it as done.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scripting", meta = (Categories = "History"))
	FGameplayTagContainer HistoryTags;
	
	/// Game designer's description of what this Item Data is supposed to do. This is only used
	/// in debug messages so it can be safely left untranslated if needed. Use shift+enter to
	/// create a new-line.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Scripting", meta = (MultiLine=true))
	FText Description;
	
	/// How to treat the source item when using the default implementation
	/// of OnItemUseSuccess.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", meta = (Categories = "Item.Treatment"))
	FGameplayTagContainer SourceItemTreatmentTags;
	
	/// The <b>first</b> item required for the action to successfully initiate.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", meta = (Categories = "Item.Kind"))
	FGameplayTag SourceItem;

	/// How to treat the target item when using the default implementation
	/// of OnItemUseSuccess.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", meta = (Categories = "Item.Treatment"))
	FGameplayTagContainer TargetItemTreatmentTags;
	
	/// The <b>second item</b> required for the action to successfully initiate, or null.
	/// There must be either this set, or the <code>HotSpot</code> set.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", meta = (Categories = "Item.Kind"))
	FGameplayTag TargetItem;

	/// If the items are used the other way around, does it matter?
	/// Using a TV remote on a TV might matter - may want to say "false"
	/// since "Use TV on remote" does not sound right. But if it's
	/// like mixing two ingredients "Use flour on water" should be the
	/// same as "Use water on flour" - so use <code>true</code> which
	/// is the default.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	bool CanSwapSourceAndTarget = true;

	/// What text to bark if the action is successful
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BarkText")
	FText ActionSuccessBarkText;

	/// What sound to play if the action is successful
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	USoundWave *ActionSuccessSound;

	/// What text to bark if the action fails
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BarkText")
	FText ActionFailureBarkText;
	
	/// What sound to play if the action fails
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	USoundWave *ActionFailureSound;

	/// If the tool is used on a consumable, what should the result be
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	FName ToolResultItem;

	/// How many seconds should the players command remain highlighted
	/// and input locked after executing it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction")
	float InteractionTimeout = 5.0f;
};
