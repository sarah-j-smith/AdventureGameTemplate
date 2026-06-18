#pragma once

class UItem;

struct FItemListTestUtils
{
    static TArray<FName> GetItemDataForTestName(const FString& TestName);

    static int GetItemToRemoveForTestName(const FString& TestName);

    static TArray<FString> GetEnumStringsAfterRemovalForTestName(const FString& TestName);

    static TArray<FString> GetEnumStringsOfResult(TArray<UItem *> Result);
};
