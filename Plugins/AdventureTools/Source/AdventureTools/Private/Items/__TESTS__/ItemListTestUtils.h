#pragma once

#include "ItemKind.h"

class UInventoryItem;

struct FItemListTestUtils
{
public:
    static TArray<EItemKind> GetItemDataForTestName(const FString& TestName);

    static int GetItemToRemoveForTestName(const FString& TestName);

    static TArray<FString> GetEnumStringsAfterRemovalForTestName(const FString& TestName);

    static TArray<FString> GetEnumStringsOfResult(TArray<UInventoryItem *> Result);
};
