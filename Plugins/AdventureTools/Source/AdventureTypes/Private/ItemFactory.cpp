// (c) 2026 Storybridge Games


#include "ItemFactory.h"

#include "Item.h"

UItemFactory::UItemFactory()
{
	bCreateNew = true;
	SupportedClass = UItem::StaticClass();
}

UObject* UItemFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
                                        EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UItem *NewItem = NewObject<UItem>(InParent, InClass, InName, Flags | RF_Transactional);
	return NewItem;
}
