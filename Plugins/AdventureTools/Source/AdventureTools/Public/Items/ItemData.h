#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct ADVENTURETOOLS_API FItemData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemData)
	TSubclassOf<UObject> ItemClass = UInventoryItem::StaticClass();
};
