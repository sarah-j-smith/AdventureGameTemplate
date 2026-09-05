#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "MockAGHUD.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Player/CommandManager.h"
#include "Tests/AutomationCommon.h"

BEGIN_DEFINE_SPEC(CommandManagerButtonSpec, "Private.Player.CommandManager.ButtonTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
FTestWorldWrapper WorldWrapper;
UWorld* World;
UMockAghud *MockHUD;
ACommandManager *CommandManager;

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
			
			UAdventureGameInstance *AGI = NewObject<UAdventureGameInstance>(World);
			World->SetGameInstance(AGI);
			AGI->AddToRoot();  // Prevent garbage collection
			AGI->Init();
		
			CommandManager = Cast<ACommandManager>(World->SpawnActor(ACommandManager::StaticClass()));
			CommandManager->AddToRoot();
			
			// Don't create the default HUD, we'll use the test mock
			CommandManager->bDisableHUD = true;
			MockHUD = NewObject<UMockAghud>(World);
			MockHUD->AddToRoot();
			MockHUD->BindCommandHandlers(CommandManager);
			
			WorldWrapper.BeginPlayInTestWorld();
		});

		AfterEach([this]()
		{
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
	
		It("Do a verb works", [this]()
		{
			// Press the LookAt verb button
			CommandManager->AssignVerb(EVerbType::LookAt);
	
			TestEqual("Should set current verb to look at, and state to 'VerbPending'", 
				CommandManager->CurrentCommand, EPlayerCommand::VerbPending);
	
			TestEqual("Should set current verb to look at, and state to 'VerbPending'", 
				CommandManager->CurrentVerb, EVerbType::LookAt);			
		});
	});
}

#endif
