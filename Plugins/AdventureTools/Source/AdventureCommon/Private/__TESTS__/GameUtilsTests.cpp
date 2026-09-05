#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "CoreMinimal.h"
#include "GameUtils.h"

BEGIN_DEFINE_SPEC(GameUtilsTests, "Private.Tests.GameUtilsTests",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

	TMap<FString, TPair<FString, int>> TestData = {
		{TEXT("22_medium_one_line"), {TEXT("This is the first line"), 6}}, // 22 chars - 20-30 bucket - one line
		{TEXT("9_short_two_lines"), {TEXT("What!\nNo!"), 3}}, // 9 chars - < 10 short bucket - two lines
		{TEXT("34_long_three_lines"), {TEXT("Now is the\nwinter of our\ndiscontent"), 11}},
		// 34 chars - 30-45 bucket - three lines
		{
			TEXT("80_xxlong_four_lines"),
			{TEXT("This is the first line\nHello World!\nThis is the third line\nThis is the fourth line"), 29}
		} // 80 chars - 45+ overflow bucket - four lines
	};

END_DEFINE_SPEC(GameUtilsTests)

void GameUtilsTests::Define()
{
	Describe("GameUtilsTests", [this]()
	{
		for (auto Case : TestData)
		{
			const auto [ArgString, ExpectedDuration] = Case.Value;
			It(FString::Printf(TEXT("Duration for: %s"), *Case.Key),
			   [this, ArgString, ExpectedDuration]()
			   {
				   float GotDuration = FGameUtils::GetDisplayTimeForString(ArgString);
				   TestEqual(TEXT("Duration is correct"), (int)GotDuration, ExpectedDuration);
			   });
		};
	});
}

#endif
