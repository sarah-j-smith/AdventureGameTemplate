#include "BarkLine.h"
#include "BarkText.h"
#include "BarkTextSUT.h"
#include "Components/VerticalBox.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

#define NOT_COMPLETED -99
#define WAS_INTERRUPTED -86

IMPLEMENT_SIMPLE_AUTOMATION_TEST(BarkTextTest, "Private.Tests.BarkTextTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool BarkTextTest::RunTest(const FString& Parameters)
{
	// This will get cleaned up when it leaves scope
	FTestWorldWrapper WorldWrapper;
	WorldWrapper.CreateTestWorld(EWorldType::Game);
	UWorld* World = WorldWrapper.GetTestWorld();

	if (World)
	{
		WorldWrapper.BeginPlayInTestWorld();
		UBarkTextSUT *SUTBuilder = CreateWidget<UBarkTextSUT>(World, UBarkTextSUT::StaticClass());
		UBarkText *BarkText = SUTBuilder->CreateBarkText(World);
		
		int CompletedUID = NOT_COMPLETED;
		BarkText->BarkRequestCompleteDelegate.AddLambda([&CompletedUID](int UID){ CompletedUID = UID; });
		BarkText->BarkRequestInterruptedDelegate.AddLambda([&CompletedUID](int UID){ CompletedUID = WAS_INTERRUPTED; });
		
		const TArray<FText>& BarkTextArray = {
			FText::FromString("This is the first line"),
			FText::FromString("Hello World!"),
			FText::FromString("This is the second line"),
			FText::FromString("This is the third line")
		};
		const FBarkRequest *BarkRequest = FBarkRequest::CreatePlayerMultilineRequest(BarkTextArray);
		
		TestEqual(TEXT("Correct display time"), BarkRequest->GetDuration(), 20.0f);
		
		BarkText->AddBarkRequest(BarkRequest);
		
		TestEqual(TEXT("Should capture correct display time for whole request"), 25.0f, 
			BarkRequest->GetDuration());
		
		TestTrue(TEXT("IsPlayerRequest"), BarkText->IsPlayerRequest());
		
		{
			// Tick a beat to allow the display to put up the first line, check its there
			BarkText->DoTick(1.0f);
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(0));
			TestEqual(TEXT("Displayed first line"), BarkLine->Text->GetText().ToString(),
				FText::FromString("This is the first line").ToString());
		}

		{
			// Tick a beat to allow the display to put up the first line, check its there
			float ElapsedTime = BarkRequest->GetDuration() - 1.0f;
			int ResultCode = SUTBuilder->TickUntil(1.0f, ElapsedTime, [](UBarkText *BarkTextArg)
			{
				return BarkTextArg->BarkContainer->GetChildrenCount() == 2;
			});
			TestEqual(TEXT("Expected new text displayed in time"), ResultCode, TARGET_REACHED);
			TestLessThan(TEXT("Should be some time left"), BarkText->ElapsedTime(), ElapsedTime);
			
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(1));
			TestEqual(TEXT("Displayed second line"), BarkLine->Text->GetText().ToString(),
		FText::FromString("This is the first line").ToString());

			BarkText->DoTick(1.0f);
		}
		
		{
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(2));
			TestEqual(TEXT("Displayed third line"), BarkLine->Text->GetText().ToString(),
		FText::FromString("This is the first line").ToString());

			BarkText->DoTick(1.0f);
		}

		{
			/// When the number of lines is > G_MAX_BARK_LINES then we start scrolling old lines
			/// off the top, and the new line should appear at the bottom
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(2));
			TestEqual(TEXT("Displayed fourth line"), BarkLine->Text->GetText().ToString(),
		FText::FromString("This is the first line").ToString());
		}
		    
		WorldWrapper.ForwardErrorMessages(this);
		return !HasAnyErrors();
	}
	return false;
}
