// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "ItemFactory.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETYPES_API UItemFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UItemFactory();
	
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
									  EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
