#pragma once

#include "CoreMinimal.h"

#include "PromptDisplayDataUnit.generated.h"

USTRUCT(BlueprintType)
struct FPromptDisplayDataUnit
{
	GENERATED_USTRUCT_BODY()
	
	TArray<FText> PromptText;
	bool HasBeenSelected;
};
