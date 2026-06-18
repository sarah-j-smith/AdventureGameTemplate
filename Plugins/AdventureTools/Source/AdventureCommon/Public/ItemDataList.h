#pragma once

#include "CoreMinimal.h"

#include "ItemDataWrapper.h"

#include "ItemDataList.generated.h"

class UStoryAction;

USTRUCT(BlueprintType)
struct ADVENTURECOMMON_API FItemDataList
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ItemAction", DisplayName="Data Items", meta = (TitleProperty = "ItemDataTitle"))
    TArray<FItemDataWrapper> ItemDataRecords;

    UStoryAction* GetItemDataAssetForAction(EVerbType Verb) const;
};
