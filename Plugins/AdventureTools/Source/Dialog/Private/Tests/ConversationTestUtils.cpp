#include "ConversationTestUtils.h"


#include "PromptData.h"

using CharData = const TCHAR*[3][3];
CharData PromptText = {
    {TEXT("Hello. My name is Inigo Montoya.")},
    {TEXT("You killed my father"), TEXT("Prepare to die")},
    {TEXT("I just cut off your hand"), TEXT("What do you say now?")}
};

CharData ResponseText = {
    {TEXT("I am Count Rugen.")},
    {TEXT("You are that sad little boy"), TEXT("Now I will have to kill you"), TEXT("How tragic")},
    {TEXT("Bye"), TEXT("Damn this door is stuck"), TEXT("Can I plead for mercy?")}
};

FName SomeEvent = TEXT("Some event");

FPromptData FConversationTestUtils::CreatePromptData(
    int32 Number, int32 SubNumber, bool Visible, bool HasBeenSelected,
    bool SingleUse, const TCHAR* PromptTextIn[], unsigned int PromptTextCount,
    const TCHAR* NPCResponse[], unsigned int NPCResponseCount,
    MockDataTable* SwitchDataTable = nullptr, bool EndsConversation = false, const TCHAR* Event = nullptr
)
{
    FPromptData PromptData;
    PromptData.PromptNumber = Number;
    PromptData.PromptSubNumber = SubNumber;
    PromptData.Visible = Visible;
    PromptData.HasBeenSelected = HasBeenSelected;
    PromptData.SingleUse = SingleUse;
    {
        TArray<FText> PromptTextArray;
        for (unsigned int i = 0; i < PromptTextCount; i++)
        {
            PromptTextArray.Add(FText::FromString(PromptTextIn[i]));
        }
        PromptData.PromptText = PromptTextArray;
    }
    {
        TArray<FText> NPCResponseTextArray;
        for (unsigned int i = 0; i < NPCResponseCount; i++)
        {
            NPCResponseTextArray.Add(FText::FromString(NPCResponse[i]));
        }
        PromptData.NPCResponse = NPCResponseTextArray;
    }
    PromptData.SwitchTopic = SwitchDataTable;
    PromptData.EndsConversation = EndsConversation;
    PromptData.Event = Event;
    return PromptData;
}

struct FSequenceDataSpec
{
    int PromptIndex;
    int PromptSubIndex;
    bool SingleUse;
};

FConversationData CreateSequenceData(std::vector<FSequenceDataSpec> SequenceDataSpecs)
{
    FConversationData ConversationData;

    for (unsigned int i = 0; i < SequenceDataSpecs.size(); i++)
    {
        int StringsIndex = i % 3;
        ConversationData.ConversationPromptArray.Add(
            FConversationTestUtils::CreatePromptData(SequenceDataSpecs[i].PromptIndex,
                                                     SequenceDataSpecs[i].PromptSubIndex,
                                                     true, false,
                                                     SequenceDataSpecs[i].SingleUse, PromptText[StringsIndex], 1,
                                                     ResponseText[StringsIndex], 1,
                                                     nullptr, false, TEXT("Some event")
            ));
    }
    return ConversationData;
}

FConversationData FConversationTestUtils::CreateData()
{
    return CreateSequenceData(
        {
            {0, 0, false},
            {1, 0, true},
            {1, 1, false}
        });
}

FConversationData FConversationTestUtils::CreateEmptyTextRowData()
{
    FConversationData ConversationData;

    // "Prompt 1 (row 1) has empty text"
    const TCHAR* EmptyText2[1] = {}; // empty array

    ConversationData.ConversationPromptArray.Add(
        CreatePromptData(0, 0, true, false,
                         false, PromptText[0], 1,
                         ResponseText[0], 1,
                         nullptr, false, TEXT("Some event")
        ));
    ConversationData.ConversationPromptArray.Add(
        CreatePromptData(1, 0, true, false,
                         true, EmptyText2, 0,
                         ResponseText[1], 2,
                         nullptr, false, TEXT("")
        )
    );
    return ConversationData;
}

FConversationData FConversationTestUtils::CreateEmptyTextItemData()
{
    FConversationData ConversationData;

    // "Prompt 2 (row 2) has empty text"
    const TCHAR* EmptyItem2[] = {TEXT("You killed my father"), TEXT("")}; // empty element

    ConversationData.ConversationPromptArray.Add(
        CreatePromptData(0, 0, true, false,
                         false, PromptText[0], 1,
                         ResponseText[0], 1,
                         nullptr, false, TEXT("Some event")
        ));
    ConversationData.ConversationPromptArray.Add(
        CreatePromptData(1, 0, true, false,
                         true, PromptText[1], 2,
                         ResponseText[1], 2,
                         nullptr, false, TEXT("")
        )
    );
    ConversationData.ConversationPromptArray.Add(
        CreatePromptData(2, 0, true, false,
                         true, EmptyItem2, 2,
                         ResponseText[2], 2,
                         nullptr, false, TEXT("")
        )
    );
    return ConversationData;
}


FConversationData FConversationTestUtils::CreateSingleUseErrorData()
{
    FConversationData ConversationData;

    return CreateSequenceData({
        {0, 0, false},
        {1, 0, false},
        {1, 1, true}
    });
}

FConversationData FConversationTestUtils::CreateBadOrderSequenceData()
{
    // Prompt (row 4) Invalid sequence  Prev [ 2 / 0 ], Next [ Prompt: 1 / Sub-prompt: 0 ]
    return CreateSequenceData({
        {0, 0, false},
        {1, 0, true},
        {1, 1, false},
        {2, 0, false},
        {1, 0, false}
    });
}

FConversationData FConversationTestUtils::CreateBadSubSequenceData()
{
    // Prompt (row 2) Invalid sequence  Prev [ 1 / 0 ], Next [ Prompt: 1 / Sub-prompt: 0 ]
    return CreateSequenceData({
        {0, 0, false},
        {1, 0, true},
        {1, 0, false},
        {2, 0, false},
        {3, 0, false}
    });
}

FConversationData FConversationTestUtils::CreateLongBadSequenceData()
{
    // Prompt (row 5) Invalid sequence  Prev [ 2 / 1 ], Next [ Prompt: 2 / Sub-prompt: 0 ]
    return CreateSequenceData({
        {0, 0, false},
        {1, 0, true},
        {1, 1, false},
        {2, 0, true},
        {2, 1, true},
        {2, 0, false},
        {3, 0, false},
        {4, 0, false},
        {5, 0, false},
        {6, 0, true},
        {6, 1, false},
        {7, 0, false}
    });
}
