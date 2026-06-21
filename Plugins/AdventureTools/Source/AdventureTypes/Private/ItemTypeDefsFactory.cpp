// (c) 2026 Storybridge Games


#include "ItemTypeDefsFactory.h"

#include "AdventureGameplayTags.h"
#include "ItemTypeDefs.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"

UItemTypeDefsFactory::UItemTypeDefsFactory()
{
	bCreateNew = true;
	SupportedClass = UItemTypeDefs::StaticClass();
}

UObject* UItemTypeDefsFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	UItemTypeDefs *Defs = NewObject<UItemTypeDefs>(InParent, InClass, InName, Flags | RF_Transactional);
	FItemTypeDef Pickle = FItemTypeDef(AdventureGameplayTags::Item_Kind_Pickle, LOCTEXT("ItemTypeDefsFactoryNew_Pickle_Desc", "Pickle")); 
	Defs->ItemTypeDefs.Add(Pickle);
	return Defs;
}

#undef LOCTEXT_NAMESPACE