#include "BarkTestUtils.h"

void BarkTestUtils::GetTestDataForParam(const FString& ParamName, TArray<FText>& BarkTextArray)
{
	if (ParamName == TEXT("Simply_one_line_of_text")) { GetTestDataBarkSingleLine(BarkTextArray); return; }
	if (ParamName == TEXT("Two_lines_one_very_long")) { GetTestDataVeryLongLine(BarkTextArray); return; }
	if (ParamName == TEXT("Four_short_lines_of_text")) { GetTestDataBarkArrays(BarkTextArray); return; }
	if (ParamName == TEXT("Four_lines_of_text_one_long")) { GetMultilineTestDataBarkArrays(BarkTextArray); return; }
	throw std::invalid_argument(TCHAR_TO_UTF8(*ParamName));
}

int BarkTestUtils::GetTestCountForParam(const FString& ParamName)
{
	if (ParamName == TEXT("Simply_one_line_of_text")) return 1;
	if (ParamName == TEXT("Two_lines_one_very_long")) return 3;
	if (ParamName == TEXT("Four_short_lines_of_text")) return 4;
	if (ParamName == TEXT("Four_lines_of_text_one_long")) return 6;
	throw std::invalid_argument(TCHAR_TO_UTF8(*ParamName));
}

int BarkTestUtils::GetTestElapsedTimeForParam(const FString& ParamName)
{
	if (ParamName == TEXT("Simply_one_line_of_text")) return 5.0f;
	if (ParamName == TEXT("Two_lines_one_very_long")) return 17.0f;
	if (ParamName == TEXT("Four_short_lines_of_text")) return 22.0f;
	if (ParamName == TEXT("Four_lines_of_text_one_long")) return 43.0f;
	throw std::invalid_argument(TCHAR_TO_UTF8(*ParamName));
}

void BarkTestUtils::GetTestDataBarkSingleLine(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("Just a single line!")
	});
}

void BarkTestUtils::GetTestDataVeryLongLine(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("How now you secret black and midnight hags!"), // 43 chars > BARK_LINE_WIDTH of 30
		FText::FromString("This is the first line"),
	});
}
void BarkTestUtils::GetTestDataBarkArrays(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("This is the first line"),
		FText::FromString("Hello World!"),
		FText::FromString("This is the third line"),
		FText::FromString("This is the 4th line")
	});
}
	
void BarkTestUtils::GetMultilineTestDataBarkArrays(TArray<FText>& BarkTextArray)
{
	BarkTextArray.Empty();
	BarkTextArray.Append({
		FText::FromString("This is the first line"),
		FText::FromString("Hello World!\nI have line feeds in this longer line.\nHmmm."),
		FText::FromString("This is the third line"),
		FText::FromString("This is the 4th line")
	});
}