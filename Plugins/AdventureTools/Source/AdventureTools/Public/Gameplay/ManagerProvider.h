// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ManagerProvider.generated.h"

class UItemManager;
class ACommandManager;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UManagerProvider : public UObject
{
	GENERATED_BODY()
public:
	/// Get a pointer to an Item Manager instance
	UFUNCTION(BlueprintCallable, Category = "ItemManager", meta = (WorldContext = "WorldContextObject"))
	UItemManager *GetItemManager(UObject *WorldContextObject);
    
	/// Get a pointer to a Command Manager instance
	UFUNCTION(BlueprintCallable, Category = "ItemManager", meta = (WorldContext = "WorldContextObject"))
	ACommandManager *GetCommandManager(UObject *WorldContextObject);
};
