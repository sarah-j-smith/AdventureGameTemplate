// (c) 2026 Storybridge Games


#include "StoryActionAssetDefinition.h"

#include "AdventureTypesStyles.h"
#include "StoryAction.h"

#define LOCTEXT_NAMESPACE "FAdventureTypesModule"
#define COL( C ) float(C) / 255.0f

TSoftClassPtr<UObject> UStoryActionAssetDefinition::GetAssetClass() const
{
	return UStoryAction::StaticClass();
}

FText UStoryActionAssetDefinition::GetAssetDisplayName() const
{
	return LOCTEXT("ItemTypeDefs_AssetDisplayName", "Story Action Asset");
}

TConstArrayView<FAssetCategoryPath> UStoryActionAssetDefinition::GetAssetCategories() const
{
	static const FAssetCategoryPath CategoryPaths[] = { FText::FromString("Adventure Tools") };
	return CategoryPaths;
}

const FSlateBrush* UStoryActionAssetDefinition::GetIconBrush(const FAssetData& InAssetData,
															  const FName InClassName) const
{
	return FAdventureTypesStyles::Get().GetBrush("ClassIcon.StoryAction");
}

FLinearColor UStoryActionAssetDefinition::GetAssetColor() const
{
	return FLinearColor( COL(0xD7), COL(0x91), COL(0x15), 1.0f);
}

#undef COL
#undef LOCTEXT_NAMESPACE 