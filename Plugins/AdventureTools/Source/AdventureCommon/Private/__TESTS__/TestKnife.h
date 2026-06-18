// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "TestKnife.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURECOMMON_API UTestKnife : public UItem
{
	GENERATED_BODY()
	
	UTestKnife()
	{
		ItemTypeDef = "Knife";
	}
};
