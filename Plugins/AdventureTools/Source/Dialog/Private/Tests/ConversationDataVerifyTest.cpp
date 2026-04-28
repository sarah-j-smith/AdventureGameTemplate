#include "ConversationData.h"
#include "Misc/AutomationTest.h"

#include "ConversationTestUtils.h"

// IMPLEMENT_SIMPLE_AUTOMATION_TEST(ConversationDataVerifyTest, "Dialog.ConversationDataVerifyTest",
//                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

IMPLEMENT_COMPLEX_AUTOMATION_TEST(ConversationDataVerifyTest, "AdventureGame.Dialog.ConversationDataVerifyTest",
     EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

FConversationData GetDataForTestName(const FString& TestName)
{
    if (TestName == "EmptyTextRow") return FConversationTestUtils::CreateEmptyTextRowData();
    if (TestName == "EmptyTextItem") return FConversationTestUtils::CreateEmptyTextItemData();
    if (TestName == "BadSubSequence") return FConversationTestUtils::CreateBadSubSequenceData();
    if (TestName == "BadOrderSequence") return FConversationTestUtils::CreateBadOrderSequenceData();
    if (TestName == "LongBadSequence") return FConversationTestUtils::CreateLongBadSequenceData();
    if (TestName == "SingleUseError") return FConversationTestUtils::CreateSingleUseErrorData();
    UE_LOG(LogDialog, Fatal, TEXT("Illegal test name"));
    return FConversationTestUtils::CreateEmptyTextRowData();
}

FString GetErrorMessageForTestName(const FString& TestName)
{
    const FString Expected = FString::Printf(TEXT("Prompt %d (row %d) has empty text"), 1, 1);
    if (TestName == "EmptyTextRow") return FString::Printf(EMPTY_TEXT_ERROR, 1, 1);
    if (TestName == "EmptyTextItem") return FString::Printf(EMPTY_TEXT_ERROR, 2, 2);
    if (TestName == "BadSubSequence") return FString::Printf(SEQUENCE_ERROR, 2, 1, 0, 1, 0);
    if (TestName == "BadOrderSequence") return FString::Printf(SEQUENCE_ERROR, 4, 2, 0, 1, 0);
    if (TestName == "LongBadSequence") return FString::Printf(SEQUENCE_ERROR, 5, 2, 1, 2, 0);
    if (TestName == "SingleUseError") return FString::Printf(SINGLE_USE_ERROR, 1, 1, 1, 2);
    UE_LOG(LogDialog, Fatal, TEXT("Illegal test name"));
    return TEXT("Illegal test name");
}

uint8 GetCountForTestName(const FString& TestName)
{
    if (TestName == "EmptyTextRow") return 2;
    if (TestName == "EmptyTextItem") return 3;
    if (TestName == "BadSubSequence") return 4;
    if (TestName == "BadOrderSequence") return 3;
    if (TestName == "LongBadSequence") return 8;
    if (TestName == "SingleUseError") return 2;
    UE_LOG(LogDialog, Fatal, TEXT("Illegal test name"));
    return 0;
}

void ConversationDataVerifyTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Append({ TEXT("Empty Text Row"), TEXT("Empty Text Item"),
        TEXT("Bad Sub-Sequence"),TEXT("Bad Order Sequence"),TEXT("Long Bad Sequence"),TEXT("Single Use Error") });
    OutTestCommands.Append({ TEXT("EmptyTextRow"), TEXT("EmptyTextItem"),
        TEXT("BadSubSequence"),TEXT("BadOrderSequence"),TEXT("LongBadSequence"), TEXT("SingleUseError") });
}

bool ConversationDataVerifyTest::RunTest(const FString& Parameters)
{
    if (Parameters == "SingleUseError")
    {
        UE_LOG(LogDialog, Display, TEXT("Hi mom"));
    };
    
    FConversationData ConversationData = GetDataForTestName(Parameters);
    uint8 ExpectedCount = GetCountForTestName(Parameters);
    TestEqual(TEXT("Count is correct"), ConversationData.PromptsAvailableCount(), ExpectedCount);

    TArray<FPromptData> PromptsToDisplay;
    ConversationData.DisplayPrompts(PromptsToDisplay);

    FString ErrorText;
    bool Result = ConversationData.Validate(ErrorText);

    const FString Expected = GetErrorMessageForTestName(Parameters);
    TestEqualSensitive(Parameters, ErrorText, Expected);

    // Make the test pass by returning true, or fail by returning false.
    return true;
}
