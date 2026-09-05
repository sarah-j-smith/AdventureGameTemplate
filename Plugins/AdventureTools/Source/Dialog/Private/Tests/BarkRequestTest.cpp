#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "BarkText.h"
#include "Misc/AutomationTest.h"

BEGIN_DEFINE_SPEC(BarkRequestTest, "Private.Tests.BarkRequestTest",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

TMap<FString, TPair<int, int>> TestData = {
	{TEXT("Simply_one_line_of_text"), {1, 4}},
	{TEXT("Two_lines_one_very_long"), {3, 15}},
	{TEXT("Four_short_lines_of_text"), {4, 18}},
	{TEXT("Four_lines_of_text_one_long"), {6, 37}},
	{TEXT("Game_text"), {4, 20}}
};

void GetTestDataBarkSingleLine(TArray<FText>& BarkTextArray);
void GetTestDataVeryLongLine(TArray<FText>& BarkTextArray);
void GetTestDataBarkArrays(TArray<FText>& BarkTextArray);
void GetMultilineTestDataBarkArrays(TArray<FText>& BarkTextArray);
void GetGameText(TArray<FText>& BarkTextArray);
void GetTestDataForParam(const FString& ParamName, TArray<FText>& BarkTextArray);

END_DEFINE_SPEC(BarkRequestTest)

void BarkRequestTest::Define()
{
	Describe("BarkRequest", [this]()
	{
		for (auto Case: TestData)
		{
			auto [ ExpectedLineCount, ExpectedDuration ] = Case.Value;
			const FString TestId = Case.Key;
			It(TestId, [this, ExpectedLineCount, ExpectedDuration, TestId ]()
			{
				TArray<FText> BarkTextArray;
				GetTestDataForParam(TestId, BarkTextArray);
				const FBarkRequest *BarkRequest = FBarkRequest::CreatePlayerMultilineRequest(BarkTextArray);
		
				TestNotEqual(TEXT("UID has been set to valid value"), BarkRequest->GetUID(), BARK_UID_NONE);
			
				TestEqual(TEXT("Should have correct display time for whole request"), 
					BarkRequest->GetDuration(), static_cast<float>(ExpectedDuration));
			
				TestTrue(TEXT("IsPlayerRequest"), BarkRequest->IsPlayer());
		
				TestEqual(TEXT("All lines added"), BarkRequest->GetLineCount(), ExpectedLineCount);
				
				TestEqual(TEXT("Color is correctly set to player color"), BarkRequest->GetColor(), 
					G_Player_Default_Text_Colour.ToFColor(true));
			});
		}
	});
}


void BarkRequestTest::GetTestDataForParam(const FString& ParamName, TArray<FText>& BarkTextArray)
{
	if (ParamName == TEXT("Simply_one_line_of_text")) { GetTestDataBarkSingleLine(BarkTextArray); return; }
	if (ParamName == TEXT("Two_lines_one_very_long")) { GetTestDataVeryLongLine(BarkTextArray); return; }
	if (ParamName == TEXT("Four_short_lines_of_text")) { GetTestDataBarkArrays(BarkTextArray); return; }
	if (ParamName == TEXT("Four_lines_of_text_one_long")) { GetMultilineTestDataBarkArrays(BarkTextArray); return; }
	if (ParamName == TEXT("Game_text")) { GetGameText(BarkTextArray); return; }
	UE_LOG(LogTemp, Fatal, TEXT("Unexpected test parameter %s"), *ParamName);
}

void BarkRequestTest::GetTestDataBarkSingleLine(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("Just a single line!")
	});
}

void BarkRequestTest::GetTestDataVeryLongLine(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("How now you secret black and midnight hags!"), // 43 chars > BARK_LINE_WIDTH of 30
		FText::FromString("This is the first line"),
	});
}

void BarkRequestTest::GetTestDataBarkArrays(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("This is the first line"),
		FText::FromString("Hello World!"),
		FText::FromString("This is the third line"),
		FText::FromString("This is the 4th line")
	});
}
	
void BarkRequestTest::GetMultilineTestDataBarkArrays(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("This is the first line"),
		FText::FromString("Hello World!\nI have line feeds in this longer line.\nHmmm."),
		FText::FromString("This is the third line"),
		FText::FromString("This is the 4th line")
	});
}

void BarkRequestTest::GetGameText(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("A tatty old recruitment poster"),
		FText::FromString("the Global Marines."),
		FText::FromString("Something odd about it."),
		FText::FromString("What's this lump?"),
	});
}


#endif
