#include "CoreMinimal.h"

#include "GameUtils.h"
#include "Misc/AutomationTest.h"

// https://minifloppy.it/posts/2024/automated-testing-specs-ue5/
//  Should re-write these to use spec tests
//  https://dev.epicgames.com/documentation/unreal-engine/automation-spec-in-unreal-engine?application_version=5.5&lang=en-US


// #if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_COMPLEX_AUTOMATION_TEST(GameUtilsTests, "Private.Tests.GameUtilsTests",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

uint32 GetExpectedDurationForString(const FString &ParamName)
{
	if (ParamName == TEXT("22_medium_one_line"))   return 7;     // 22 chars - 20-30 bucket - one line
	if (ParamName == TEXT("9_short_two_lines"))    return 5;     // 9 chars - < 10 short bucket - two lines
	if (ParamName == TEXT("34_long_three_lines"))  return 14;    // 34 chars - 30-45 bucket - three lines
	if (ParamName == TEXT("80_xxlong_four_lines"))  return 33;   // 80 chars - 45+ overflow bucket - four lines
	UE_LOG(LogTemp, Fatal, TEXT("Unexpected test parameter %s"), *ParamName);
	return 0;
}

FString GetArgumentForString(const FString &ParamName)
{
	if (ParamName == TEXT("22_medium_one_line"))  
		return TEXT("This is the first line");            // 22 chars - medium bucket - one line
	if (ParamName == TEXT("9_short_two_lines"))   
		return TEXT("What!\nNo!");                        // 9 chars - short bucket - two lines
	if (ParamName == TEXT("34_long_three_lines"))
		return TEXT("Now is the\nwinter of our\ndiscontent");    // 34 chars - long bucket - three lines
	if (ParamName == TEXT("80_xxlong_four_lines"))  // 80 chars - extra long bucket - four lines
		return TEXT("This is the first line\nHello World!\nThis is the third line\nThis is the fourth line");
	UE_LOG(LogTemp, Fatal, TEXT("Unexpected test parameter %s"), *ParamName);
	return TEXT("INVALID");
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
	float GotDuration = FGameUtils::GetDisplayTimeForString(GetArgumentForString(Parameters));
	float ExpectedDuration = GetExpectedDurationForString(Parameters);
	TestEqual(TEXT("Duration is correct"), GotDuration, ExpectedDuration);
	return true;
}

// #endif
