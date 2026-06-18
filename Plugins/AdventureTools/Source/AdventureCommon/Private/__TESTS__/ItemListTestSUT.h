// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Inventory.h"

#include "UObject/Object.h"

#include "ItemListTestSUT.generated.h"

/**
 * UItemList is the SUT. Usually should not make changes and should test
 * the same thing that is in prod. But also important to avoid polluting the prod
 * version with test code. So we make a few minimally invasive additions
 * changes to make a more testable UItemList. To init the DataTable we
 * need to inject an ObjectFinder into the constructor.
 *
 * The alternative would be to path in the BP_ItemList which has the value
 * of <code>InventoryDataTable</code> set in its BluePrint. The problem there
 * is the object finder only works in a constructor, and we want to avoid
 * creating an instructor for the test. This is a reasonable compromise.
 */
UCLASS()
class UItemListTestSut: public UInventory
{
    GENERATED_BODY()
public:
    UItemListTestSut();
};