// (c) 2025 Sarah Smith

#include "Enums/InteractionType.h"

FString InteractionGetDescriptiveString(const EInteractionType InteractionType)
{
	switch (InteractionType) {
	case EInteractionType::Interact:
		return "Interact";
	case EInteractionType::Sit:
		return "Sit";
	case EInteractionType::Climb:
		return "Climb";
	case EInteractionType::TurnLeft:
		return "Turn Left";
	case EInteractionType::TurnRight:
		return "Turn Right";
	default:
		return "None";
	}
}

FString TimeDirectionGetDescriptiveString(EInteractTimeDirection InteractTimeDirection)
{
	switch (InteractTimeDirection) {
	case EInteractTimeDirection::Forward:
		return "Forward";
	case EInteractTimeDirection::Backward:
		return "Backward";
	}
	return "Never";
}
