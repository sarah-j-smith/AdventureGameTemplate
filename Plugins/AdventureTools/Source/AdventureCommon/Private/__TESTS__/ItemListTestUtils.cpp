#include "ItemListTestUtils.h"

#include "Item.h"

TArray<FName> FItemListTestUtils::GetItemDataForTestName(const FString& TestName)
{
    if (TestName.StartsWith("ThreeItem_Remove" )) return {
        "Knife",
        "Pickle",
        "PickleKey"
    };
    return {};
}

int FItemListTestUtils::GetItemToRemoveForTestName(const FString& TestName)
{
    if (TestName == "ThreeItem_Remove_Middle_Item") return 1;
    if (TestName == "ThreeItem_Remove_First_Item") return 0;
    if (TestName == "ThreeItem_Remove_Last_Item") return 2;
    UE_LOG(LogTemp, Fatal, TEXT("Test name not found"));
    return -1;
}

TArray<FString> FItemListTestUtils::GetEnumStringsAfterRemovalForTestName(const FString& TestName)
{
    TArray<FName> Items = GetItemDataForTestName(TestName);
    Items.RemoveAt(GetItemToRemoveForTestName(TestName));
    TArray<FString> ItemEnumStrings;
    for (FName Item : Items)
    {
        ItemEnumStrings.Add(Item.ToString());
    }
    return ItemEnumStrings;
}

TArray<FString> FItemListTestUtils::GetEnumStringsOfResult(TArray<UItem*> Result)
{
    TArray<FString> ItemEnumStrings;
    for (UItem* Item : Result)
    {
        ItemEnumStrings.Add(Item->ItemTypeDef.GetTagLeafName().ToString());
    }
    return ItemEnumStrings;
}
