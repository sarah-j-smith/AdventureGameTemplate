#pragma once

#include "CoreMinimal.h"

#include "WalkDirection.generated.h"

UENUM(BlueprintType)
enum class EWalkDirection: uint8
{
	Up     = 0 UMETA(DisplayName = "Up"),
	Left   = 1 UMETA(DisplayName = "Left"),
	Right  = 2 UMETA(DisplayName = "Right"),
	Down   = 3 UMETA(DisplayName = "Down"),
};

