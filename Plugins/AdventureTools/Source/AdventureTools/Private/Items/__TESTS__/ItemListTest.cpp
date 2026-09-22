#include "AdventureGameplayTags.h"
#include "IItemTableProvider.h"
#include "Inventory.h"
#include "Items/ItemData.h"
#include "ItemTypeDefs.h"
#include "Provider.h"
#include "TestKnife.h"
#include "TestPickle.h"
#include "TestPickleKey.h"

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

/**
 * Tests for the FInventory class, which replaced the UItemList.
 */
BEGIN_DEFINE_SPEC(ItemListTest, "AdventureGame.Items.ItemListTest",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

public:
	TArray<FString> OutTestCommands = {
		TEXT("ThreeItem_Remove_Middle_Item"),
		TEXT("ThreeItem_Remove_First_Item"),
		TEXT("ThreeItem_Remove_Last_Item")
	};

	class MockItemTableProvider : public IItemTableProvider
	{
		UItemTypeDefs* GetItemTypeDefs()
		{
			UItemTypeDefs* DataTable = NewObject<UItemTypeDefs>();
			{
				FItemTypeDef Def;
				Def.UniqueName = AdventureGameplayTags::Item_Kind_PickleKey;
				Def.bEnabled = true;
				Def.bValid = true;
				Def.TypeDescription = FText::FromString(TEXT("A key that is made from pickle"));
				Def.Item = NewObject<UTestPickleKey>(DataTable);
				DataTable->ItemTypeDefs.Add(Def);
			}
			{
				FItemTypeDef Def;
				Def.UniqueName = AdventureGameplayTags::Item_Kind_Pickle;
				Def.bEnabled = true;
				Def.bValid = true;
				Def.TypeDescription = FText::FromString(TEXT("A tasty little sour treat"));
				Def.Item = NewObject<UTestPickle>(DataTable);
				DataTable->ItemTypeDefs.Add(Def);
			}
			{
				FItemTypeDef Def;
				Def.UniqueName = AdventureGameplayTags::Item_Kind_Knife;
				Def.bEnabled = true;
				Def.bValid = true;
				Def.TypeDescription = FText::FromString(TEXT("Sharp and cutting"));
				Def.Item = NewObject<UTestKnife>(DataTable);
				DataTable->ItemTypeDefs.Add(Def);
			}
			return DataTable;
		}

		UDataTable* GetDataTable()
		{
			UDataTable* Table = NewObject<UDataTable>();
			const FName PickleKeyName = FGameplayTag(AdventureGameplayTags::Item_Kind_PickleKey).GetTagLeafName();
			FItemData PickleKeyData;
			PickleKeyData.ItemClass = TSoftClassPtr<UInventoryItem>(UInventoryItem::StaticClass());
			Table->AddRow(PickleKeyName, PickleKeyData);
			return Table;
		}

	public:
		virtual ~MockItemTableProvider() override = default;

		virtual TSoftObjectPtr<UDataTable> GetItemBehavioursTable() override
		{
			return GetDataTable();
		};

		virtual TSoftObjectPtr<UItemTypeDefs> GetItemDefinitionsTable() override
		{
			return GetItemTypeDefs();
		};

		virtual void RegisterWithGameInstance(UItem* InventoryItem) override
		{
		};

		virtual void UnregisterFromGameInstance(UItem* InventoryItem) override
		{
		};
	};

	TArray<FName> GetItemDataForTestName(const FString& TestName)
	{
		if (TestName.StartsWith("ThreeItem_Remove"))
		{
			return {
				"Knife",
				"Pickle",
				"PickleKey"
			};
		}
		return {};
	}

	int GetItemToRemoveForTestName(const FString& TestName)
	{
		if (TestName == "ThreeItem_Remove_Middle_Item")
		{
			return 1;
		}
		if (TestName == "ThreeItem_Remove_First_Item")
		{
			return 0;
		}
		if (TestName == "ThreeItem_Remove_Last_Item")
		{
			return 2;
		}
		UE_LOG(LogTemp, Fatal, TEXT("Test name not found"));
		return -1;
	}

	TArray<FString> GetEnumStringsAfterRemovalForTestName(const FString& TestName)
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

	TArray<FString> GetEnumStringsOfResult(TArray<UItem*> Result)
	{
		TArray<FString> ItemEnumStrings;
		for (UItem* Item : Result)
		{
			ItemEnumStrings.Add(Item->ItemTypeDef.GetTagLeafName().ToString());
		}
		return ItemEnumStrings;
	}

END_DEFINE_SPEC(ItemListTest)

void ItemListTest::Define()
{
	UProvider::Get()->RegisterClass<IItemTableProvider, MockItemTableProvider>();

	Describe("Inventory", [this]()
	{
		BeforeEach([this]()
		{
			// This will get cleaned up when it leaves scope
			FTestWorldWrapper WorldWrapper;
			WorldWrapper.CreateTestWorld(EWorldType::Game);
			UWorld* World = WorldWrapper.GetTestWorld();

			if (!World)
			{
				return;
			}
			WorldWrapper.BeginPlayInTestWorld();
		});
		
		for (auto Case : OutTestCommands)
		{
			It(Case, [this, Case]()
			{
				// Auto-clean up when the test exits
				TSharedPtr<FInventory> Inventory = MakeShareable<FInventory>(new FInventory);
				TestTrue(TEXT("Can the UItemList be created"), Inventory.IsValid());
				
				const TArray<FName> TestItems = GetItemDataForTestName(Case);
				const int ItemToRemove = GetItemToRemoveForTestName(Case);
				const TArray<FString> EnumStrings = GetEnumStringsAfterRemovalForTestName(Case);

				Inventory->AddItemInstanceByName(TestItems[0]);
				TestEqual(TEXT("Count must be 1"), Inventory->GetInventorySize(), 1);
				Inventory->AddItemInstanceByName(TestItems[1]);
				TestEqual(TEXT("Count must be 1"), Inventory->GetInventorySize(), 2);
				Inventory->AddItemInstanceByName(TestItems[2]);
				TestEqual(TEXT("Count must be 1"), Inventory->GetInventorySize(), 3);

				TArray<UItem*> TestInventoryItems;
				Inventory->GetInventoryItemsArray(TestInventoryItems);

				TestEqual(TEXT("Check GetInventory matches expected"), TestInventoryItems.Num(), 3);
				TestEqual(TEXT("Items returned correct"), TestInventoryItems[0]->ShortDescription.ToString(),
				          TEXT("knife"));

				Inventory->RemoveItemInstanceByName(TestItems[ItemToRemove]);

				TestEqual(TEXT("Count must be 2"), Inventory->GetInventorySize(), 2);

				TArray<UItem*> TestInventoryItemsAfterDeletion;
				Inventory->GetInventoryItemsArray(TestInventoryItemsAfterDeletion);

				TArray<FString> ResultStrings = GetEnumStringsOfResult(
					TestInventoryItemsAfterDeletion);
				TestEqual(TEXT("Count should be 2"), TestInventoryItemsAfterDeletion.Num(), 2);

				for (int i = 0; i < 2; i++)
				{
					TestEqual(TEXT("After removal expect correct items"), ResultStrings[i], EnumStrings[i]);
				}
			});
		}
	});
}

