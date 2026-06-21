// (c) 2025 Sarah Smith

#include "ItemListTestSUT.h"

#include "AdventureGameplayTags.h"
#include "ItemTypeDefs.h"
#include "TestKnife.h"
#include "TestPickle.h"
#include "TestPickleKey.h"

UItemListTestSut::UItemListTestSut()
{
    UItemTypeDefs *DataTable = CreateDefaultSubobject<UItemTypeDefs>(TEXT("InventoryDataTable"));
    {
        FItemTypeDef Def;
        Def.UniqueName = AdventureGameplayTags::Item_Kind_PickleKey;
        Def.ItemClass = UTestPickleKey::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("A key that is made from pickle"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    {
        FItemTypeDef Def;
        Def.UniqueName = AdventureGameplayTags::Item_Kind_Pickle;
        Def.ItemClass = UTestPickle::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("A tasty little sour treat"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    {
        FItemTypeDef Def;
        Def.UniqueName = AdventureGameplayTags::Item_Kind_Knife;
        Def.ItemClass = UTestKnife::StaticClass();
        Def.bEnabled = true;
        Def.bValid = true;
        Def.TypeDescription = FText::FromString(TEXT("Sharp and cutting"));
        DataTable->ItemTypeDefs.Add(Def);
    }
    
    this->InventoryDataTable = DataTable;
}
