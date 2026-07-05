// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "AdventureGameplayTags.h"
#include "TestPickleKey.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURECOMMON_API UTestPickleKey : public UItem
{
	GENERATED_BODY()
	
	UTestPickleKey()
	{
		ItemTypeDef = AdventureGameplayTags::Item_Kind_PickleKey;
		ShortDescription = FText::FromString(TEXT("pickle key"));
	}
};
