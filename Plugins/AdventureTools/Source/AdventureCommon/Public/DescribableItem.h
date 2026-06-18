// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DescribableItem.generated.h"

enum class EItemKind : uint8;
// This class does not need to be modified.
UINTERFACE()
class UDescribableItem : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ADVENTURECOMMON_API IDescribableItem
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FText GetShortDescription() const = 0;
	
	virtual FText GetLongDescription() const = 0;
	
	virtual FName GetItemKind() const = 0;
};
