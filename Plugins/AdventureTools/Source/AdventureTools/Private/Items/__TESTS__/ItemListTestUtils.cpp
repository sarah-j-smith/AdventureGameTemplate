#include "ItemListTestUtils.h"

#include "Items/InventoryItem.h"

TArray<EItemKind> FItemListTestUtils::GetItemDataForTestName(const FString& TestName)
{
    if (TestName.StartsWith("ThreeItem_Remove" )) return {
        EItemKind::Knife,
        EItemKind::Pickle,
        EItemKind::PickleKey
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
    TArray<EItemKind> Items = GetItemDataForTestName(TestName);
    Items.RemoveAt(GetItemToRemoveForTestName(TestName));
    TArray<FString> ItemEnumStrings;
    for (int i = 0; i < Items.Num(); i++)
    {
        ItemEnumStrings.Add(UEnum::GetValueAsString(Items[i]));
    }
    return ItemEnumStrings;
}

TArray<FString> FItemListTestUtils::GetEnumStringsOfResult(TArray<UInventoryItem*> Result)
{
    TArray<FString> ItemEnumStrings;
    for (int i = 0; i < Result.Num(); i++)
    {
        ItemEnumStrings.Add(UEnum::GetValueAsString(Result[i]->ItemKind));
    }
    return ItemEnumStrings;
}
