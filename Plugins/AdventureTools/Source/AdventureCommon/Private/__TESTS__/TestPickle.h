// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "TestPickle.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURECOMMON_API UTestPickle : public UItem
{
	GENERATED_BODY()
	
	UTestPickle()
	{
		ItemTypeDef = "Pickle";
	}
};
