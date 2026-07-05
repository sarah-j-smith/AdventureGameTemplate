// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "HistoryTagInterface.h"

#include "TagQuerySense.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdvGameDataFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum class ETagQueryResultType : uint8
{
	/// The history tag query PASSED and the hotspot event should continue, eg award points, give the item
	Continue = 0 UMETA(DisplayName = "Continue"),
	
	/// The history tag query BLOCKED the hotspot event, eg a bark should display eg "I already opened the chest" 
	Blocked = 1 UMETA(DisplayName = "Blocked")
};

/**
 * 
 */
UCLASS()
class ADVENTURECOMMON_API UAdvGameDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	/**
	 * Check a tag query against history tags, and continue on a success path if the check passes.
	 * If the check fails, then continue on a failure path, eg display "I already opened the chest"
	 * @param HistorySourceTags Hotspot or Item History tags to test against.
	 * @param CheckTag The tag to use as a gate, eg <code>History.Triggered.Open</code>
	 * @param TagQuerySense Whether to check for the tag being present, or absent
	 * @param TagQueryResult Resulting path for the check
	 */
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "TagQueryResult"), Category = "Flow Control")
	static void TagStateCheck(
		FGameplayTagContainer HistorySourceTags, 
		UPARAM(meta = (Categories = "History")) FGameplayTag CheckTag, 
		ETagQueryResultType &TagQueryResult,
		ETagQuerySense TagQuerySense = ETagQuerySense::TagMustBePresent);
	
	/**
	 * Check a tag to see if its present in the history tags of a given <b>object</b>; and if it's <b>not present</b>
	 * continue on a success path and <b>add the tag to the object</b> so that it will not execute the path next time.
	 * If the tag is present, then continue on a failure path, eg display "I already opened the chest"
	 * @param HistoryObject Hotspot or Item with History tags to test against.
	 * @param CheckTag The tag to use as a gate, eg <code>History.Triggered.LookAt</code>
	 * @param TagQueryResult Resulting path for the check
	 */
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "TagQueryResult"), Category = "Flow Control")
	static void HistoryTagGate(
		TScriptInterface<IHistoryTagInterface> HistoryObject, 
		UPARAM(meta = (Categories = "History")) FGameplayTag CheckTag, 
		ETagQueryResultType &TagQueryResult);
};
