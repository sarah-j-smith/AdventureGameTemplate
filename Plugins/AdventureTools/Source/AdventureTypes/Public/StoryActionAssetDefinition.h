// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "StoryActionAssetDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETYPES_API UStoryActionAssetDefinition : public UAssetDefinitionDefault
{
	GENERATED_BODY()
public:
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
	
	virtual FText GetAssetDisplayName() const override;
	
	virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
	
	virtual const FSlateBrush* GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const override;
	
	virtual FLinearColor GetAssetColor() const override;
};
