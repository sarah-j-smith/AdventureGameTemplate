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
    
    this->InventoryDataTable = DataTable;
}
