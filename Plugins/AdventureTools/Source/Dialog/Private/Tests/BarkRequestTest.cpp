#include "BarkTestUtils.h"
#include "BarkText.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(BarkRequestTest, "Private.Tests.BarkRequestTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void BarkRequestTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Append({
		TEXT("Simply one line of text"),
		TEXT("Two lines one very long"),
		TEXT("Four short lines of text"),
		TEXT("Four lines of text one long"),
	});
	OutTestCommands.Append({
		TEXT("Simply_one_line_of_text"),
		TEXT("Two_lines_one_very_long"),
		TEXT("Four_short_lines_of_text"),
		TEXT("Four_lines_of_text_one_long"),
	});
}

bool BarkRequestTest::RunTest(const FString& Parameters)
{
	TArray<FText> BarkTextArray;
	BarkTestUtils::GetTestDataForParam(Parameters, BarkTextArray);
	int ExpectedCount = BarkTestUtils::GetTestCountForParam(Parameters);
	float ElapsedTime = BarkTestUtils::GetTestElapsedTimeForParam(Parameters);
	
	FBarkRequest *BarkRequest = FBarkRequest::CreatePlayerMultilineRequest(BarkTextArray);
	
	const int GotDuration = BarkRequest->GetDuration();
	TestNotEqual(TEXT("UID has been set to valid value"), BarkRequest->GetUID(), BARK_UID_NONE);
		
	TestEqual(TEXT("Should have correct display time for whole request"), ElapsedTime, 
		BarkRequest->GetDuration());
		
	TestTrue(TEXT("IsPlayerRequest"), BarkRequest->IsPlayer());
	
	TestEqual(TEXT("All lines added"), BarkRequest->GetLineCount(), ExpectedCount);
			
	TestEqual(TEXT("Color is correctly set to player color"), BarkRequest->GetColor(), 
		G_Player_Default_Text_Colour.ToFColor(true));

	return true;
}
