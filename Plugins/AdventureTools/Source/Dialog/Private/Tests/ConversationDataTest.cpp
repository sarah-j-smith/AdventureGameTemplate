#include "ConversationData.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(ConversationDataTest, "AdventureGame.Dialog.ConversationDataTest",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

#include "ConversationTestUtils.h"

bool ConversationDataTest::RunTest(const FString& Parameters)
{
    FConversationData ConversationData = FConversationTestUtils::CreateData();

    TestEqual(TEXT("Count is correct"), ConversationData.PromptsAvailableCount(), 2);

    TArray<FPromptData> PromptsToDisplay;
    ConversationData.DisplayPrompts(PromptsToDisplay);
    TestEqual(TEXT("Query members number"), PromptsToDisplay[0].PromptNumber, 0);
    TestEqual(TEXT("Query members text"), PromptsToDisplay[0].PromptNumber, 0);
    TestEqual(TEXT("Query members second number"), PromptsToDisplay[1].PromptNumber, 1);
    TestEqual(TEXT("Query members second txt"), PromptsToDisplay[1].PromptNumber, 1);

    ConversationData.MarkPromptSelected(0, 0);
    ConversationData.DisplayPrompts(PromptsToDisplay);
    TestTrue(TEXT("Query members selected"), PromptsToDisplay[0].HasBeenSelected);
    TestTrue(TEXT("Query members selected"), PromptsToDisplay[0].Visible);
    TestEqual(TEXT("Count is correct"), ConversationData.PromptsAvailableCount(), 2);
    
    TestEqual(TEXT("Query members text"), PromptsToDisplay[0].PromptNumber, 0);
    TestEqual(TEXT("Query members second number"), PromptsToDisplay[1].PromptNumber, 1);
    TestEqual(TEXT("Query members second txt"), PromptsToDisplay[1].PromptNumber, 1);

    return true;
}
