// (c) 2025 Sarah Smith

#include "ItemListTestSUT.h"

#include "ItemTypeDefs.h"
#include "TestKnife.h"
#include "TestPickle.h"
#include "TestPickleKey.h"

UItemListTestSut::UItemListTestSut()
{
    UItemTypeDefs *DataTable = CreateDefaultSubobject<UItemTypeDefs>(TEXT("InventoryDataTable"));
    {
        FItemTypeDef Def;
        Def.UniqueName = "PickleKey";
        Def.ItemClass = UTestPickleKey::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("A key that is made from pickle"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    {
        FItemTypeDef Def;
        Def.UniqueName = "Pickle";
        Def.ItemClass = UTestPickle::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("A tasty little sour treat"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    {
        FItemTypeDef Def;
        Def.UniqueName = "Knife";
        Def.ItemClass = UTestKnife::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("Sharp and cutting"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    
    this->InventoryDataTable = DataTable;
}
