// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemManagerProvider.generated.h"

class UItemManager;
class ACommandManager;

UINTERFACE(NotBlueprintable)
class UItemManagerProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class ADVENTURETOOLS_API IItemManagerProvider
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    /// Get a pointer to an Item Manager instance
    UFUNCTION(BlueprintCallable, Category = "ItemManager")
    virtual UItemManager *GetItemManager();
    
    /// Get a pointer to a Command Manager instance
    UFUNCTION(BlueprintCallable, Category = "ItemManager")
    virtual ACommandManager *GetCommandManager();
};
