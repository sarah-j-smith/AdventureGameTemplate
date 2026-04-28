#pragma once

#include "CoreMinimal.h"

#include "InteractionType.generated.h"

UENUM(BlueprintType)
enum class EInteractionType: uint8
{
	None        = 0 UMETA(DisplayName = "NONE"),
	Interact    = 1 UMETA(DisplayName = "INTERACT"),
	Sit         = 2 UMETA(DisplayName = "SIT"),
	Climb       = 3 UMETA(DisplayName = "CLIMB"),
	TurnLeft    = 4 UMETA(DisplayName = "TURN LEFT"),
	TurnRight   = 5 UMETA(DisplayName = "TURN RIGHT"),
	UseItem     = 6 UMETA(DisplayName = "USE ITEM"),
	GiveItem	= 7 UMETA(DisplayName = "GIVE ITEM"),
};

FString InteractionGetDescriptiveString(EInteractionType InteractionType);

UENUM(BlueprintType)
enum class EInteractTimeDirection: uint8
{
	Forward    = 0 UMETA(DisplayName = "FORWARD"),
	Backward   = 1 UMETA(DisplayName = "BACKWARD"),
};

FString TimeDirectionGetDescriptiveString(EInteractTimeDirection InteractTimeDirection);