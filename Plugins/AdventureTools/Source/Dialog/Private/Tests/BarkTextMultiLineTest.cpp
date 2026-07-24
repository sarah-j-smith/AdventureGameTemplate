#include "BarkLine.h"
#include "BarkTestUtils.h"
#include "BarkText.h"
#include "BarkTextSUT.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(BarkTextMultiLineTest, "Private.Tests.BarkTextMultiLineTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool BarkTextMultiLineTest::RunTest(const FString& Parameters)
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
		BarkTestUtils::GetTestDataBarkArrays(BarkTextArray);
		FBarkRequest *BarkRequest = SUTBuilder->SetupTestData(BarkTextArray);
		
		// Tick a beat to allow the display to put up the first line, check its there
		BarkText->DoTick(1.0f);

		UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(0));
		TestEqual(TEXT("Displayed first line"), BarkLine->Text->GetText().ToString(),
			FText::FromString("This is the first line").ToString());
		TestEqual(TEXT("Contains one string"), 
			BarkText->BarkContainer->GetChildrenCount(), 1);

		float ElapsedTime = BarkRequest->GetDurationForLine(0);
		int Result_1 = SUTBuilder->TickUntilChildCount(2, ElapsedTime);

		// Tick through the allowed time to show the next lines
		TestEqual(TEXT("Expected new text displayed in time"), Result_1, TARGET_REACHED);
		TestLessThan(TEXT("Should be some time left"), BarkText->ElapsedTime(), ElapsedTime);
		
		BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(1));
		TestEqual(TEXT("Displayed second line"), BarkLine->Text->GetText().ToString(),
	BarkTextArray[1].ToString());
		
		ElapsedTime = BarkRequest->GetDurationForLine(1); 
		int Result_2 = SUTBuilder->TickUntilChildCount(3, ElapsedTime);
		
		TestEqual(TEXT("Expected new text displayed in time"), Result_2, TARGET_REACHED);
		ElapsedTime = BarkRequest->GetDurationForLine(2);
		TestLessEqual(TEXT("Should be third line timer"), BarkText->ElapsedTime(), ElapsedTime);
		
		BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(2));
		TestEqual(TEXT("Displayed third line"), BarkLine->Text->GetText().ToString(),
	BarkTextArray[2].ToString());
		TestEqual(TEXT("Contains three strings"), BarkText->BarkContainer->GetChildrenCount(), 3);

		ElapsedTime = BarkRequest->GetDurationForLine(2);
		int Result_3 = SUTBuilder->TickUntilTextExists(BarkTextArray[3], ElapsedTime);
		
		TestEqual(TEXT("Expected new text displayed in time"), Result_3, TARGET_REACHED);
		TestLessThan(TEXT("Should be some time left"), BarkText->ElapsedTime(), ElapsedTime);
		TestEqual(TEXT("Contains three strings"), BarkText->BarkContainer->GetChildrenCount(), 3);
		
		WorldWrapper.ForwardErrorMessages(this);
		return !HasAnyErrors();
	}
	return false;
}
