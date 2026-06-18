// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "StoryActionFactory.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETYPES_API UStoryActionFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UStoryActionFactory();
	
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, 
		EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
