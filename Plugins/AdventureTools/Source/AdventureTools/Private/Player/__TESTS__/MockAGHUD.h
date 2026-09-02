#pragma once

#include "MockAGHUD.generated.h"

class ACommandManager;
class UAdventureGameInstance;
class AAdventureGameModeBase;
class UInteractionNotifier;
enum class ESaveGameStatus : uint8;

UCLASS(MinimalAPI)
class UMockAghud: public UObject
{
	GENERATED_BODY()
public:
	void BeginDestroy() override;
	
	/// Subscribe to messages for command & action updates to show in the interaction display.
	void BindCommandHandlers(ACommandManager *CommandManager);

	/// Subscribe to messages for changes in the player inventory to show in the inventory UI
	void BindInventoryHandlers(UAdventureGameInstance* AdventureGameInstance);

	/// Subscribe to messages for changes in the player score to show in the score UI
	void BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode);

	/// Subscribe to messages for commands from the player, used to open or close
	/// the conversation UI, or signal that a UI interaction happened to dismiss the
	/// current bark or NPC conversation
	void BindNotifierHandlers(UInteractionNotifier* Notifier);
	
	UFUNCTION()
	void BeginActionEvent() {}
	
	UFUNCTION()
	void UpdateInteractionTextEvent() {}
	
	UFUNCTION()
	void InterruptActionEvent() {}
    
	UFUNCTION()
	void OnUserInteracted() {}
	
	UFUNCTION()
	void UpdateSaveGameIndicatorEvent(ESaveGameStatus SaveGameStatus, bool Success);

	UFUNCTION()
	void UpdateInventoryTextEvent();
	
	UFUNCTION()
	void HandleInventoryChanged(FName ItemKind, EItemDisposition Disposition);

	UFUNCTION()
	void HandleScoreChanged(int32 Score);
};
