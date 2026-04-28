#pragma once

#include "CoreMinimal.h"

#include "VerbType.generated.h"

UENUM(BlueprintType)
enum class EVerbType: uint8
{
	Give    = 0 UMETA(DisplayName = "GIVE"),
	Open    = 1 UMETA(DisplayName = "OPEN"),
	Close   = 2 UMETA(DisplayName = "CLOSE"),
	PickUp  = 3 UMETA(DisplayName = "PICK_UP"),
	LookAt  = 4 UMETA(DisplayName = "LOOK_AT"),
	TalkTo  = 5 UMETA(DisplayName = "TALK_TO"),
	Use     = 6 UMETA(DisplayName = "USE"),
	Push    = 7 UMETA(DisplayName = "PUSH"),
	Pull    = 8 UMETA(DisplayName = "PULL"),
	WalkTo  = 9 UMETA(DisplayName = "WALK_TO"),
	UseItem = 10 UMETA(DisplayName = "USE_ITEM"),
	GiveItem = 11 UMETA(DisplayName = "GIVE_ITEM"),
};

ADVENTURECOMMON_API FText VerbGetDescriptiveString(EVerbType Verb);
