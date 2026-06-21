// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "ItemTypeDef.h"
#include "UObject/Object.h"
#include "ItemTypeDefs.generated.h"

/**
 * Container for definitions of all the items that can exist in a game. Each game should have 
 * exactly one of these, and it should contain unique names for each item, eg "Knife", "Ham_Sandwich";
 * and the human-readable names for each item, eg "Knife", "Ham sandwich".
 * 
 * @see ItemTypeDefsFactory in the AdventureTypes module 
 */
UCLASS()
class ADVENTURECOMMON_API UItemTypeDefs : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Adventure Tools", meta=(ShowOnlyInnerProperties))
	TArray<FItemTypeDef> ItemTypeDefs;
	
	FItemTypeDef FindDefByName(const FName& ItemName) const;
	FItemTypeDef FindItemByClass(const FString& ClassName) const;
};
