#include "BarkLine.h"
#include "BarkText.h"
#include "BarkTextSUT.h"
#include "Components/VerticalBox.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(BarkTextTest, "Private.Tests.BarkTextTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void GetTestDataBarkArrays(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("This is the first line"),
		FText::FromString("Hello World!"),
		FText::FromString("This is the third line"),
		FText::FromString("This is the 4th line")
	});
}

void BarkTextTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Append({
		TEXT("Create bark request"),
		TEXT("Single line of text"),
		TEXT("Two lines of text"),
		TEXT("Fill the bark holder - three lines"),
		TEXT("Cause bark holder to scroll - four lines"),
		TEXT("Bark completed normally"),
		TEXT("Bark interrupted")
	});
	OutTestCommands.Append({
		TEXT("Create_bark_request"),
		TEXT("Handle_one_line_string"),
		TEXT("Handle_two_line_string"),
		TEXT("Handle_three_line_string"),
		TEXT("Handle_four_line_string")
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
		TArray<FText> BarkTestData;
		GetTestDataBarkArrays(BarkTestData);
		FBarkRequest *BarkRequest = SUTBuilder->SetupTestData(BarkTestData);
		
		if (Parameters == TEXT("Create_bark_request"))
		{
			const int GotDuration = BarkRequest->GetDuration();
			TestNotEqual(TEXT("UID has been set to valid value"), BarkRequest->GetUID(), BARK_UID_NONE);
		
			TestEqual(TEXT("Should have correct display time for whole request"), 22.0f, 
				BarkRequest->GetDuration());
		
			TestTrue(TEXT("IsPlayerRequest"), BarkText->IsPlayerRequest());
			
			TestEqual(TEXT("Color is correctly set to player color"), BarkRequest->GetColor(), 
				G_Player_Default_Text_Colour.ToFColor(true));
			return !HasAnyErrors();
		}

		// Tick a beat to allow the display to put up the first line, check its there
		BarkText->DoTick(1.0f);

		if (Parameters == TEXT("Handle_one_line_string"))
		{
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(0));
			TestEqual(TEXT("Displayed first line"), BarkLine->Text->GetText().ToString(),
				FText::FromString("This is the first line").ToString());
			TestEqual(TEXT("Contains one string"), 
				BarkText->BarkContainer->GetChildrenCount(), 1);
			return !HasAnyErrors();
		}

		float ElapsedTime = BarkRequest->GetDurationForLine(0);
		int Result_1 = SUTBuilder->TickUntilChildCount(2, ElapsedTime);

		if (Parameters == TEXT("Handle_two_line_string"))
		{
			// Tick through the allowed time to show the next lines
			TestEqual(TEXT("Expected new text displayed in time"), Result_1, TARGET_REACHED);
			TestLessThan(TEXT("Should be some time left"), BarkText->ElapsedTime(), ElapsedTime);
			
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(1));
			TestEqual(TEXT("Displayed second line"), BarkLine->Text->GetText().ToString(),
		BarkTestData[1].ToString());
			return !HasAnyErrors();
		}
		
		ElapsedTime = BarkRequest->GetDurationForLine(1); 
		int Result_2 = SUTBuilder->TickUntilChildCount(3, ElapsedTime);
		
		if (Parameters == TEXT("Handle_three_line_string"))
		{
			TestEqual(TEXT("Expected new text displayed in time"), Result_2, TARGET_REACHED);
			ElapsedTime = BarkRequest->GetDurationForLine(2);
			TestLessEqual(TEXT("Should be third line timer"), BarkText->ElapsedTime(), ElapsedTime);
			
			UBarkLine *BarkLine = Cast<UBarkLine>(BarkText->BarkContainer->GetChildAt(2));
			TestEqual(TEXT("Displayed third line"), BarkLine->Text->GetText().ToString(),
		BarkTestData[2].ToString());
			TestEqual(TEXT("Contains three strings"), BarkText->BarkContainer->GetChildrenCount(), 3);

			return !HasAnyErrors();
		}

		ElapsedTime = BarkRequest->GetDurationForLine(2);
		int Result_3 = SUTBuilder->TickUntilTextExists(BarkTestData[3], ElapsedTime);
		
		if (Parameters == TEXT("Handle_four_line_string"))
		{
			TestEqual(TEXT("Expected new text displayed in time"), Result_3, TARGET_REACHED);
			TestLessThan(TEXT("Should be some time left"), BarkText->ElapsedTime(), ElapsedTime);
			TestEqual(TEXT("Contains three strings"), BarkText->BarkContainer->GetChildrenCount(), 3);
			return !HasAnyErrors();
		}
		
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
			return !HasAnyErrors();
		}
		
		if (Parameters == TEXT("Bark_interrupted"))
		{
			BarkText->OnUserInteracted();
			TestEqual(TEXT("Text completed fired and set the value"), 
		SUTBuilder->CompletedUID, BarkRequest->GetUID());
			TestEqual(TEXT("Text completed fired and set the value"), 
				SUTBuilder->FinishedReason, EBarkRequestFinishedReason::Interruption);
			return !HasAnyErrors();
		}
		
		WorldWrapper.ForwardErrorMessages(this);
		return !HasAnyErrors();
	}
	return false;
}
