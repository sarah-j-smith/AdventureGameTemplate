#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "ConversationData.h"
#include "PromptData.h"

class MockDataTable : public UDataTable
{
    std::vector<std::string> ConversationData;
};

class FConversationTestUtils
{
public:
    static FPromptData CreatePromptData(
        int32 Number, int32 SubNumber, bool Visible, bool HasBeenSelected,
        bool SingleUse, const TCHAR* PromptText[], unsigned int PromptTextCount,
        const TCHAR* NPCResponse[], unsigned int NPCResponseCount,
        MockDataTable* SwitchDataTable, bool EndsConversation, const TCHAR* Event
    );

    static FConversationData CreateData();

    static FConversationData CreateEmptyTextRowData();

    static FConversationData CreateEmptyTextItemData();

    static FConversationData CreateBadSubSequenceData();

    static FConversationData CreateBadOrderSequenceData();
    
    static FConversationData CreateSingleUseErrorData();
    
    static FConversationData CreateLongBadSequenceData();
};
