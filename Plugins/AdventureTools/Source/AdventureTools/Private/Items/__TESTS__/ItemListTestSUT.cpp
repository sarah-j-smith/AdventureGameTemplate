// (c) 2025 Sarah Smith


#include "ItemListTestSUT.h"

UItemListTestSut::UItemListTestSut()
{
    ConstructorHelpers::FObjectFinder<UDataTable> InventoryDataTableLoader(
        TEXT("/Script/Engine.DataTable'/Game/PointAndClick/Blueprints/Inventory/DT_ItemList.DT_ItemList'"));
    if (InventoryDataTableLoader.Succeeded())
    {
        InventoryDataTable = InventoryDataTableLoader.Object;
    }
}