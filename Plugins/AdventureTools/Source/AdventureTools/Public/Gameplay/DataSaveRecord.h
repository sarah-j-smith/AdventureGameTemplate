#pragma once
#include "GameplayTagContainer.h"

#include "DataSaveRecord.generated.h"

USTRUCT()
struct FDataSaveRecord
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FString ObjectName;

    UPROPERTY()
    FString LevelName;

    UPROPERTY()
    FGameplayTagContainer Tags;
};
