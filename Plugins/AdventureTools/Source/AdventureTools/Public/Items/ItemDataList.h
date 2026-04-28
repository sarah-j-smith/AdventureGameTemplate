#pragma once

#include "CoreMinimal.h"

#include "ItemDataWrapper.h"

#include "ItemDataList.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct ADVENTURETOOLS_API FItemDataList
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", DisplayName="Data Items", meta = (TitleProperty = "ItemDataTitle"))
    TArray<FItemDataWrapper> ItemDataRecords;

    UItemDataAsset* GetItemDataAssetForAction(EVerbType Verb) const;
};
