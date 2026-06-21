// (c) 2026 Storybridge Games


#include "ItemTypeDefs.h"

FItemTypeDef UItemTypeDefs::FindDefByName(const FName& ItemName) const
{
	const FItemTypeDef *Def = ItemTypeDefs.FindByPredicate([ItemName](const FItemTypeDef& ItemTypeDef)
	{
		return ItemTypeDef.UniqueName.GetTagLeafName() == ItemName;
	});
	if (Def == nullptr)
	{
		// Return an invalid ItemTypeDef with bValid === false
		return FItemTypeDef();
	}
	return FItemTypeDef(*Def);
}

FItemTypeDef UItemTypeDefs::FindItemByClass(const FString& ClassName) const
{
	const FItemTypeDef *Def = ItemTypeDefs.FindByPredicate([ClassName](const FItemTypeDef& ItemTypeDef) {
		const UClass *ItemClassResolved = ItemTypeDef.ItemClass.Get();
		return ItemClassResolved && ItemClassResolved->GetName() == ClassName;
	});
	if (Def == nullptr)
	{
		// Return an invalid ItemTypeDef with bValid === false
		return FItemTypeDef();
	}
	return FItemTypeDef(*Def);
}
