#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "ButtonFixture.h"
#include "MockAdventureControllerProvider.h"
#include "MockAGHUD.h"
#include "Gameplay/AdventureGameInstance.h"
#include "HUD/ItemSlot.h"
#include "Player/CommandManager.h"
#include "Player/ItemManager.h"
#include "Tests/AutomationCommon.h"
#include "GameFramework/Character.h"
#include "Player/AdventureCharacter.h"
#include "Player/AdventurePlayerController.h"
#include "Player/Puck.h"

BEGIN_DEFINE_SPEC(CommandManagerButtonSpec, "Private.Player.CommandManager.ButtonTest",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
	FTestWorldWrapper WorldWrapper;
UWorld* World;
UMockAghud *MockHUD;
ACommandManager *CommandManager;
UButtonFixture *ButtonFixture;

END_DEFINE_SPEC(CommandManagerButtonSpec)

void CommandManagerButtonSpec::Define()
{
	Describe("CommandManagerButton", [this]()
	{
		BeforeEach([this]()
		{
			WorldWrapper.CreateTestWorld(EWorldType::Game);
			World = WorldWrapper.GetTestWorld();
			TestNotNull("Cannot run tests without world", World);
			
			UGameInstance *AGI = NewObject<UGameInstance>(World);
			World->SetGameInstance(AGI);
			AGI->AddToRoot();  // Prevent garbage collection
			AGI->Init();
		
			CommandManager = Cast<ACommandManager>(World->SpawnActor(ACommandManager::StaticClass()));
			CommandManager->AddToRoot();
			
			MockHUD = CreateWidget<UMockAghud>(World, UMockAghud::StaticClass());
			MockHUD->AddToRoot();
			
			// Don't create the default HUD, we'll use the test mock
			CommandManager->bDisableHUD = true;
			CommandManager->SetAdventureGameHUD(MockHUD);
			
			WorldWrapper.BeginPlayInTestWorld();
			ButtonFixture = CreateWidget<UButtonFixture>(World, UButtonFixture::StaticClass());
		});

		AfterEach([this]()
		{
			ButtonFixture = nullptr;
			WorldWrapper.EndPlayInTestWorld();
			UGameInstance *AGI = World->GetGameInstance();
			AGI->Shutdown();
			
			WorldWrapper.ForwardErrorMessages(this);
			MockHUD->RemoveFromRoot();
			CommandManager->RemoveFromRoot();
			World = nullptr;
			MockHUD = nullptr;
			CommandManager = nullptr;
			WorldWrapper.DestroyTestWorld(false);
		});
	
		It("Basic verb command defaults", [this]()
		{
			TestEqual("Default verb should be walk to", CommandManager->CurrentCommand, EPlayerCommand::None);
			TestEqual("Default command should be none", CommandManager->CurrentVerb, EVerbType::WalkTo);

			// Mouse over the UI panel at the bottom of the screen
			CommandManager->UpdateMouseOverUI(true);
			TestEqual("But keep none for the command", CommandManager->CurrentCommand, EPlayerCommand::None);
			TestEqual("Once moved over the panel change verb to look at", CommandManager->CurrentVerb, EVerbType::LookAt);
			
			UItemSlot *Button = ButtonFixture->CreateItemSlotButton(World);
			CommandManager->HandleInventoryItemClicked(Button);
			
			TestEqual("Should set this buttons item to the source", 
				CommandManager->ItemManager->GetSourceItem(), Button->InventoryItem);
		});
		
		It("Default look at", [this]()
		{
			UItemSlot *Button = ButtonFixture->CreateItemSlotButton(World);
			
			// Press the LookAt verb button
			CommandManager->AssignVerb(EVerbType::LookAt);
	
			TestEqual("Should set current verb to look at, and state to 'VerbPending'", 
				CommandManager->CurrentCommand, EPlayerCommand::VerbPending);
	
			TestEqual("Should set current verb to look at, and state to 'VerbPending'", 
				CommandManager->CurrentVerb, EVerbType::LookAt);
			
			CommandManager->HandleInventoryItemClicked(Button);
			TestEqual("Should look at this item on the button", 
				CommandManager->ItemManager->GetSourceItem(), Button->InventoryItem);
		});
	});
}

#endif
