// (c) 2026 Storybridge Games


#include "ItemAssetDefinition.h"

#include "AdventureTypesStyles.h"
#include "Item.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"
#define COL( C ) float(C) / 255.0f

TSoftClassPtr<UObject> UItemAssetDefinition::GetAssetClass() const
{
	return UItem::StaticClass();
}

FText UItemAssetDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("ItemTypeDefs_AssetDisplayName", "Item");
}

TConstArrayView<FAssetCategoryPath> UItemAssetDefinition::GetAssetCategories() const
{
	static const FAssetCategoryPath CategoryPaths[] = { FText::FromString("Adventure Tools") };
	return CategoryPaths;
}

const FSlateBrush* UItemAssetDefinition::GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const
{
	return FAdventureTypesStyles::Get().GetBrush("ClassIcon.Item");
}

FLinearColor UItemAssetDefinition::GetAssetColor() const
{
	return FLinearColor( COL(0xD7), COL(0x91), COL(0x15), 1.0f);
}

#undef COL
#undef LOCTEXT_NAMESPACE 