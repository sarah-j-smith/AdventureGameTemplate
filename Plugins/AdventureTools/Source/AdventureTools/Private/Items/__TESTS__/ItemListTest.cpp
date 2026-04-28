#include "ItemListTestSUT.h"
#include "ItemListTestUtils.h"
#include "Items/InventoryItem.h"
#include "Items/ItemList.h"

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_COMPLEX_AUTOMATION_TEST(ItemListTest, "AdventureGame.Items.ItemListTest",
                                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

void ItemListTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
    OutBeautifiedNames.Append({
        TEXT("Three Items, Remove Middle Item"),
        TEXT("Three Items, Remove First Item"),
        TEXT("Three Items, Remove Last Item") });
    OutTestCommands.Append({
        TEXT("ThreeItem_Remove_Middle_Item"),
        TEXT("ThreeItem_Remove_First_Item"),
        TEXT("ThreeItem_Remove_Last_Item") });
}

bool ItemListTest::RunTest(const FString& Parameters)
{
    // This will get cleaned up when it leaves scope
    FTestWorldWrapper WorldWrapper;
    WorldWrapper.CreateTestWorld(EWorldType::Game);
    UWorld* World = WorldWrapper.GetTestWorld();

    if (!World) return false;
    WorldWrapper.BeginPlayInTestWorld();
    
    const TArray<EItemKind> TestItems = FItemListTestUtils::GetItemDataForTestName(Parameters);
    const int ItemToRemove = FItemListTestUtils::GetItemToRemoveForTestName(Parameters);
    const TArray<FString> EnumStrings = FItemListTestUtils::GetEnumStringsAfterRemovalForTestName(Parameters);
    UItemListTestSut *ItemList = NewObject<UItemListTestSut>(World, UItemListTestSut::StaticClass(),
        FName(Parameters + TEXT("-ItemList")));

    TestNotNull(TEXT("Can the UItemList be created"), ItemList);
    TestNotNull(TEXT("Did the UDataTable get initialised"), ItemList->InventoryDataTable);

    ItemList->AddItemToInventory(TestItems[0]);
    TestEqual(TEXT("Count must be 1"), ItemList->InventorySize, 1);
    ItemList->AddItemToInventory(TestItems[1]);
    TestEqual(TEXT("Count must be 1"), ItemList->InventorySize, 2);
    ItemList->AddItemToInventory(TestItems[2]);
    TestEqual(TEXT("Count must be 1"), ItemList->InventorySize, 3);

    TArray<UInventoryItem*> TestInventoryItems;
    ItemList->GetInventoryItemsArray(TestInventoryItems);

    TestEqual(TEXT("Check GetInventory matches expected"), TestInventoryItems.Num(), 3);
    TestEqual(TEXT("Items returned correct"), TestInventoryItems[0]->ShortDescription.ToString(),
        TEXT("knife"));

    ItemList->RemoveItemKindFromInventory(TestItems[ItemToRemove]);

    TestEqual(TEXT("Count must be 2"), ItemList->InventorySize, 2);

    TArray<UInventoryItem*> TestInventoryItemsAfterDeletion;
    ItemList->GetInventoryItemsArray(TestInventoryItemsAfterDeletion);

    TArray<FString> ResultStrings = FItemListTestUtils::GetEnumStringsOfResult(TestInventoryItemsAfterDeletion);
    TestEqual(TEXT("Count should be 2"), TestInventoryItemsAfterDeletion.Num(), 2);

    for (int i = 0; i < 2; i++)
    {
        TestEqual(TEXT("After removal expect correct items"), ResultStrings[i], EnumStrings[i]);
    }
    
    // Make the test pass by returning true, or fail by returning false.
    return true;
}
