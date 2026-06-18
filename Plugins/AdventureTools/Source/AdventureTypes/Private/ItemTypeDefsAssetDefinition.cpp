// (c) 2026 Storybridge Games


#include "ItemTypeDefsAssetDefinition.h"

#include "AdventureTypesStyles.h"
#include "ItemTypeDefs.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"
#define COL( C ) float(C) / 255.0f

TSoftClassPtr<UObject> UItemTypeDefsAssetDefinition::GetAssetClass() const
{
	return UItemTypeDefs::StaticClass();
}

FText UItemTypeDefsAssetDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("ItemTypeDefs_AssetDisplayName", "Item Type Definitions");
}

TConstArrayView<FAssetCategoryPath> UItemTypeDefsAssetDefinition::GetAssetCategories() const
{
	static const FAssetCategoryPath CategoryPaths[] = { FText::FromString("Adventure Tools") };
	return CategoryPaths;
}

const FSlateBrush* UItemTypeDefsAssetDefinition::GetIconBrush(const FAssetData& InAssetData,
                                                              const FName InClassName) const
{
	return FAdventureTypesStyles::Get().GetBrush("ClassIcon.ItemTypeDefs");
}

FLinearColor UItemTypeDefsAssetDefinition::GetAssetColor() const
{
	return FLinearColor( COL(0xD7), COL(0x91), COL(0x15), 1.0f);
}

#undef COL
#undef LOCTEXT_NAMESPACE 
