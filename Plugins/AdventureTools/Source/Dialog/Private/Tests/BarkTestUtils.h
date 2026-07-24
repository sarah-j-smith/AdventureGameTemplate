#pragma once

class BarkTestUtils
{
public:
	/// Simply one line of text, less than BARK_LINE_WIDTH
	static void GetTestDataBarkSingleLine(TArray<FText>& BarkTextArray);
	
	/// Has 2 array elements of FText, but the first line (index 0) 
	/// is 43 characters long, so longer than BARK_LINE_WIDTH
	/// So should expand to 3 lines in the request
	static void GetTestDataVeryLongLine(TArray<FText>& BarkTextArray);
	
	/// Has 4 array elements of simple FText, all less than BARK_LINE_WIDTH
	static void GetTestDataBarkArrays(TArray<FText>& BarkTextArray);

	/// Has 4 array elements of FText, but the second line (index 1) has 2 line
	/// feeds so after parsing should expand to 6 lines in the request.
	static void GetMultilineTestDataBarkArrays(TArray<FText>& BarkTextArray);
	
	static void GetGameText(TArray<FText>& BarkTextArray);
	
	static void GetTestDataForParam(const FString& ParamName, TArray<FText>& BarkTextArray);
	
	static int GetTestCountForParam(const FString& ParamName);

	static int GetTestElapsedTimeForParam(const FString& ParamName);
};
