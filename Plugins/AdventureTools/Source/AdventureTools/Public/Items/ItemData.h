#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct ADVENTURETOOLS_API FItemData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> ItemClass = UInventoryItem::StaticClass();
};
