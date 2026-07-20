#include "BarkLine.h"
#include "BarkTestUtils.h"
#include "BarkText.h"
#include "BarkTextSUT.h"
#include "Components/VerticalBox.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(BarkTextTest, "Private.Tests.BarkTextTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void BarkTextTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Append({
		TEXT("Bark completed normally"),
		TEXT("Bark interrupted")
	});
	OutTestCommands.Append({
		TEXT("Bark_normal_completion"),
		TEXT("Bark_interrupted")
	});
}

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
		
		TArray<FText> BarkTextArray;
		BarkTestUtils::GetTestDataBarkSingleLine(BarkTextArray);
		FBarkRequest *BarkRequest = SUTBuilder->SetupTestData(BarkTextArray);

		// Tick a beat to allow the display to put up the first line, check its there
		BarkText->DoTick(1.0f);

		UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(0));
		TestEqual(TEXT("Displayed first line"), BarkLine->Text->GetText().ToString(),BarkTextArray[0].ToString());
		TestEqual(TEXT("Contains one string"), 
			BarkText->BarkContainer->GetChildrenCount(), 1);
		
		if (Parameters == TEXT("Bark_normal_completion"))
		{
			SUTBuilder->TickUntil(1.0f, 10.0f, [](UBarkText *BarkText)
			{
				return BarkText->GetCurrentBarkRequest() == nullptr;
			});
			TestEqual(TEXT("Text completed fired and set the value"), 
				SUTBuilder->CompletedUID, BarkRequest->GetUID());
			TestEqual(TEXT("Text completed fired and set the value"), 
			SUTBuilder->FinishedReason, EBarkRequestFinishedReason::Timeout);
		}
		
		if (Parameters == TEXT("Bark_interrupted"))
		{
			BarkText->OnUserInteracted();
			TestEqual(TEXT("Text completed fired and set the value"), 
		SUTBuilder->CompletedUID, BarkRequest->GetUID());
			TestEqual(TEXT("Text completed fired and set the value"), 
				SUTBuilder->FinishedReason, EBarkRequestFinishedReason::Interruption);
		}
		
		WorldWrapper.ForwardErrorMessages(this);
		return !HasAnyErrors();
	}
	return false;
}
