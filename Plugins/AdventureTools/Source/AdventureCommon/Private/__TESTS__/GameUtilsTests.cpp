#include "GameUtils.h"
#include "Misc/AutomationTest.h"

// #if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_COMPLEX_AUTOMATION_TEST(GameUtilsTests, "Private.Tests.GameUtilsTests",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

uint32 GetExpectedDurationForString(const FString &String)
{
	if (String == TEXT("22_medium_one_line"))   return 7;     // 22 chars - 20-30 bucket - one line
	if (String == TEXT("9_short_two_lines"))    return 5;     // 9 chars - < 10 short bucket - two lines
	if (String == TEXT("34_long_three_lines"))  return 14;    // 34 chars - 30-45 bucket - three lines
	if (String == TEXT("80_xxlong_four_lines"))  return 33;   // 80 chars - 45+ overflow bucket - four lines
	throw new std::invalid_argument(TCHAR_TO_UTF8(*String));
}

FString GetArgumentForString(const FString &String)
{
	if (String == TEXT("22_medium_one_line"))  
		return TEXT("This is the first line");            // 22 chars - medium bucket - one line
	if (String == TEXT("9_short_two_lines"))   
		return TEXT("What!\nNo!");                        // 9 chars - short bucket - two lines
	if (String == TEXT("34_long_three_lines"))
		return TEXT("Now is the\nwinter of our\ndiscontent");    // 34 chars - long bucket - three lines
	if (String == TEXT("80_xxlong_four_lines"))  // 80 chars - extra long bucket - four lines
		return TEXT("This is the first line\nHello World!\nThis is the third line\nThis is the fourth line");
	throw new std::invalid_argument(TCHAR_TO_UTF8(*String));
}

void GameUtilsTests::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Append({ 
		TEXT("22 chars - medium bucket - one line"),
		TEXT("9 chars - short bucket - two lines"),
		TEXT("34 chars - long bucket - three lines"),
		TEXT("80 chars - extra long bucket - four lines")
		});
	OutTestCommands.Append ({
		TEXT("22_medium_one_line"),
		TEXT("9_short_two_lines"),
		TEXT("34_long_three_lines"),
		TEXT("80_xxlong_four_lines")
		});
}

bool GameUtilsTests::RunTest(const FString& Parameters)
{
	try
	{
		float GotDuration = FGameUtils::GetDisplayTimeForString(GetArgumentForString(Parameters));
		float ExpectedDuration = GetExpectedDurationForString(Parameters);
		TestEqual(TEXT("Duration is correct"), GotDuration, ExpectedDuration);
	} 
	catch (std::exception& e)
	{
		TestFalse(TEXT("Exception thrown"), e.what());
	}
	return true;
}

// #endif
