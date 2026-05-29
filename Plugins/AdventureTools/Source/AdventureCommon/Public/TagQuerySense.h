// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Factories/EnumFactory.h"
#include "TagQuerySense.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETagQuerySense : uint8
{
	/// The given tag MUST BE THERE in the HotSpot's history tags to pass the check test
	TagMustBePresent = 0  UMETA(DisplayName = "Tag Must Be Present"),
	
	/// The given tag MUST BE ABSENT from the HotSpot's history tags to pass the check test
	TagMustNotBePresent = 1 UMETA(DisplayName = "Tag Must Not Be Present"),
};
