// (c) 2025 Sarah Smith


#include "Player/InteractTask.h"

#include "AdventureTools.h"
#include "GameUtils.h"
#include "Player/AdventurePlayerController.h"

#include "Kismet/GameplayStatics.h"

UInteractTask::UInteractTask(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
	, WorldContextObject(nullptr)
	, Interaction(EInteractionType::None)
	, TimeDirection(EInteractTimeDirection::Forward)
{
	MyUID = FGameUtils::GetUUID();
}

UInteractTask* UInteractTask::DoInteractTask(const UObject* WorldContextObject, const EInteractionType Interaction, const EInteractTimeDirection TimeDirection)
{
	UInteractTask* BlueprintNode = NewObject<UInteractTask>();
	BlueprintNode->SetWorldContextObject(WorldContextObject);
	BlueprintNode->SetInteractionType(Interaction);
	BlueprintNode->SetTimeDirection(TimeDirection);

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UInteractTask created"));
	
	// Register with the game instance to avoid being garbage collected
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UInteractTask::SetWorldContextObject(const UObject* AWorldContextObject)
{
	this->WorldContextObject = AWorldContextObject;
}

void UInteractTask::SetInteractionType(const EInteractionType InteractionType)
{
	this->Interaction = InteractionType;
}

void UInteractTask::Activate()
{
	Super::Activate();

	AAdventurePlayerController *AdventurePlayerController = GetAdventureController();

	if (!AdventurePlayerController)
	{
		UE_LOG(LogAdventureGame, Log, TEXT("UInteractTask::Activate - no adventure controller, using fake; no interact done"));
		if (UWorld *World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				FakeInteractTimer, this,
				&UInteractTask::FakeInteractTimerTimeout,
				FakeInteractTime, false);
		}
		else
		{
			FakeInteractTimerTimeout();
		}
		return;
	}

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("UInteractTask Activate - %s"), *(InteractionGetDescriptiveString(Interaction)));
	InteractionWasCompleted = Interaction;
	switch (Interaction)
	{
	case EInteractionType::Interact:
		Interact();
		break;
	case EInteractionType::Sit:
		Sit();
		break;
	case EInteractionType::Climb:
		Climb();
		break;
	case EInteractionType::TurnLeft:
		TurnLeft();
		break;
	case EInteractionType::TurnRight:
		TurnRight();
	case EInteractionType::None:
		break;
	default:
		break;
	}
	AdventurePlayerController->EndAction.AddUObject(this, &UInteractTask::InteractionCompleted);
}

void UInteractTask::FakeInteractTimerTimeout()
{
	InteractionCompleted(Interaction, MyUID, true);
}

AAdventurePlayerController* UInteractTask::GetAdventureController() const
{
	APlayerController *PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	AAdventurePlayerController *AdventurePlayerController = Cast<AAdventurePlayerController>(PlayerController);
	return AdventurePlayerController;
}

void UInteractTask::Sit()
{
	if (AAdventurePlayerController *AdventurePlayerController = GetAdventureController())
	{
		AdventurePlayerController->PlayerSit(MyUID, TimeDirection);
	}
}

void UInteractTask::Climb()
{
	if (AAdventurePlayerController *AdventurePlayerController = GetAdventureController())
	{
		AdventurePlayerController->PlayerClimb(MyUID, TimeDirection);
	}
}

void UInteractTask::TurnLeft()
{
	if (AAdventurePlayerController *AdventurePlayerController = GetAdventureController())
	{
		AdventurePlayerController->PlayerTurnLeft(MyUID, TimeDirection);
	}
}

void UInteractTask::TurnRight()
{
	if (AAdventurePlayerController *AdventurePlayerController = GetAdventureController())
	{
		AdventurePlayerController->PlayerTurnRight(MyUID, TimeDirection);
	}
}

void UInteractTask::Interact()
{
	if (AAdventurePlayerController *AdventurePlayerController = GetAdventureController())
	{
		AdventurePlayerController->PlayerInteract(MyUID, TimeDirection);
	}
}

void UInteractTask::InteractionCompleted(EInteractionType EInteraction, int32 UID, bool bSuccess)
{
	if (EInteraction == this->Interaction && UID == MyUID)
	{
		if (bSuccess)
		{
			TaskCompleted.Broadcast();
		}
		else
		{
			TaskInterrupted.Broadcast();
		}
		SetReadyToDestroy();
	}
	else
	{
		UE_LOG(LogAdventureGame, Log, TEXT("Ignoring task completion message UID: %d - Interaction: %s"),
			UID, *(InteractionGetDescriptiveString(EInteraction)));
	}
}
