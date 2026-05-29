// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

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
	 * @param WorldContextObject Supplied by the blueprint, ignore.
	 * @param HistorySourceTags Hotspot or Item History tags to test against.
	 * @param CheckTag
	 * @param TagQuerySense
	 * @param TagQueryResult
	 *
	 * @return The created bark task will either end successfully or be interrupted. Use the appropriate pins to respond to each.
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", ExpandEnumAsExecs = "TagQueryResult"), Category = "Flow Control")
	static void TagStateCheck(const UObject* WorldContextObject, 
		FGameplayTagContainer HistorySourceTags, 
		UPARAM(meta = (Categories = "History")) FGameplayTag CheckTag, 
		ETagQueryResultType &TagQueryResult,
		ETagQuerySense TagQuerySense = ETagQuerySense::TagMustBePresent);
};
