// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Runtime/Core/Public/Misc/TypeContainer.h"
#include "Provider.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURECOMMON_API UProvider : public UObject
{
	GENERATED_BODY()
public:
	
	static TSharedRef<TTypeContainer<>> Get();
};
