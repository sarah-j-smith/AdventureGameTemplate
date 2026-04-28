#pragma once

#include "CoreMinimal.h"
#include "PromptDisplayDataUnit.h"

#include "PromptDisplayData.generated.h"

USTRUCT()
struct FPromptDisplayData
{
	GENERATED_USTRUCT_BODY()
	
	TArray<FPromptDisplayDataUnit> Units;
};