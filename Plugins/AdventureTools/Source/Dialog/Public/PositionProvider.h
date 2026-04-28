// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "PositionProvider.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPositionProvider : public UInterface
{
	GENERATED_BODY()
};

class USphereComponent;

/**
 * 
 */
class DIALOG_API IPositionProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual USphereComponent *GetPosition() const = 0;
};
