#include "MockAGHUD.h"

#include "Player/CommandManager.h"
#include "Player/ItemManager.h"
#include "HUD/AdventureGameHUD.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/AdventureGameModeBase.h"
#include "Player/InteractionNotifier.h"

void UMockAghud::BeginDestroy()
{
	Super::BeginDestroy();
	
	UE_LOG(LogTemp, Log, TEXT("####  MockAghud::BeginDestroy"));
}

void UMockAghud::BindCommandHandlers(ACommandManager* CommandManager)
{
	check(CommandManager);

	CommandManager->BeginAction.AddUObject(this, &UMockAghud::Mock_BeginActionEvent);
	CommandManager->UpdateInteractionTextDelegate.AddUObject(this, &UMockAghud::Mock_UpdateInteractionTextEvent);
	CommandManager->InterruptAction.AddUniqueDynamic(this, &UMockAghud::Mock_InterruptActionEvent);
    
	if (UItemManager *ItemManager = CommandManager->ItemManager)
	{
		ItemManager->UpdateInventoryTextDelegate.AddUObject(this, &UMockAghud::Mock_UpdateInventoryTextEvent);
	}
}

void UMockAghud::BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode)
{
	AdventureGameMode->ScoreDelegate.AddUniqueDynamic(this, &UMockAghud::Mock_HandleScoreChanged);
}

void UMockAghud::BindNotifierHandlers(UInteractionNotifier* Notifier)
{   
	Notifier->UserInteraction.AddUObject(this, &UMockAghud::Mock_OnUserInteracted);
	// Notifier->PromptListOpenRequest.AddUObject(this, &UAdventureGameHUD::ShowPromptList);
	// Notifier->PromptListCloseRequest.AddUObject(this, &UAdventureGameHUD::HidePromptList);
}
