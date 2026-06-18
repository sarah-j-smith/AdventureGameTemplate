// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"

#include "ItemTypeDefsFactory.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETYPES_API UItemTypeDefsFactory : public UFactory
{
	GENERATED_BODY()

public:
	UItemTypeDefsFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
	                                  EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
