#pragma once
#include "ItemDisposition.h"
#include "HUD/AdventureGameHUD.h"

#include "MockAGHUD.generated.h"

class ACommandManager;
class UAdventureGameInstance;
class AAdventureGameModeBase;
class UInteractionNotifier;
enum class ESaveGameStatus : uint8;

UCLASS(MinimalAPI)
class UMockAghud: public UAdventureGameHUD
{
	GENERATED_BODY()
public:
	virtual void BeginDestroy() override;
	
	/// Subscribe to messages for command & action updates to show in the interaction display.
	virtual void BindCommandHandlers(ACommandManager *CommandManager) override;

	/// Subscribe to messages for changes in the player score to show in the score UI
	virtual void BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode) override;

	/// Subscribe to messages for commands from the player, used to open or close
	/// the conversation UI, or signal that a UI interaction happened to dismiss the
	/// current bark or NPC conversation
	virtual void BindNotifierHandlers(UInteractionNotifier* Notifier) override;
	
	TArray<FString> EventsReceived;
	
	UFUNCTION()
	void Mock_BeginActionEvent() { EventsReceived.Push("BeginActionEvent"); }
	
	UFUNCTION()
	void Mock_UpdateInteractionTextEvent() { EventsReceived.Push("UpdateInteractionTextEvent"); }
	
	UFUNCTION()
	void Mock_InterruptActionEvent() { EventsReceived.Push("InterruptActionEvent"); }
    
	UFUNCTION()
	void Mock_OnUserInteracted() { EventsReceived.Push("OnUserInteracted"); }
	
	UFUNCTION()
	void Mock_UpdateSaveGameIndicatorEvent(ESaveGameStatus SaveGameStatus, bool Success) { 
		EventsReceived.Push(
		FString::Printf(TEXT("UpdateSaveGameIndicatorEvent - %s - %s"), 
			*UEnum::GetValueAsString(SaveGameStatus), 
			Success ? TEXT("Success") : TEXT("Failure"))); 
	}

	UFUNCTION()
	void Mock_UpdateInventoryTextEvent() { EventsReceived.Push("UpdateInventoryTextEvent"); }
	
	UFUNCTION()
	void Mock_HandleInventoryChanged(FName ItemKind, EItemDisposition Disposition) { 
		EventsReceived.Push(
		FString::Printf(TEXT("HandleInventoryChanged - %s - %s"), 
			*ItemKind.ToString(),
			*UEnum::GetValueAsString(Disposition))); 
	}

	UFUNCTION()
	void Mock_HandleScoreChanged(int32 Score)
	{
		EventsReceived.Push(FString::Printf(TEXT("HandleScoreChanged - %d"), Score));
	}
};
