#include "MockAGHUD.h"

#include "Player/CommandManager.h"
#include "Player/ItemManager.h"
#include "HUD/AdventureGameHUD.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "Player/InteractionNotifier.h"

void UMockAghud::BeginDestroy()
{
	UObject::BeginDestroy();
	
	UE_LOG(LogTemp, Log, TEXT("####  MockAghud::BeginDestroy"));
}

void UMockAghud::BindCommandHandlers(ACommandManager* CommandManager)
{
	check(CommandManager);

	CommandManager->BeginAction.AddUObject(this, &UMockAghud::BeginActionEvent);
	CommandManager->UpdateInteractionTextDelegate.AddUObject(this, &UMockAghud::UpdateInteractionTextEvent);
	CommandManager->InterruptAction.AddUniqueDynamic(this, &UMockAghud::InterruptActionEvent);
    
	if (UItemManager *ItemManager = CommandManager->ItemManager)
	{
		ItemManager->UpdateInventoryTextDelegate.AddUObject(this, &UMockAghud::UpdateInventoryTextEvent);
	}
}

void UMockAghud::BindInventoryHandlers(UAdventureGameInstance* AdventureGameInstance)
{
	AdventureGameInstance->PlayerInventoryChanged.AddUniqueDynamic(this, &UMockAghud::HandleInventoryChanged);
}

void UMockAghud::BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode)
{
	AdventureGameMode->ScoreDelegate.AddUniqueDynamic(this, &UMockAghud::HandleScoreChanged);
}

void UMockAghud::BindNotifierHandlers(UInteractionNotifier* Notifier)
{   
	Notifier->UserInteraction.AddUObject(this, &UMockAghud::OnUserInteracted);
	// Notifier->PromptListOpenRequest.AddUObject(this, &UAdventureGameHUD::ShowPromptList);
	// Notifier->PromptListCloseRequest.AddUObject(this, &UAdventureGameHUD::HidePromptList);
}

void UMockAghud::UpdateSaveGameIndicatorEvent(ESaveGameStatus SaveGameStatus, bool Success)
{
}

void UMockAghud::UpdateInventoryTextEvent()
{
}

void UMockAghud::HandleInventoryChanged(FName ItemKind, EItemDisposition Disposition)
{
}

void UMockAghud::HandleScoreChanged(int32 Score)
{
}
