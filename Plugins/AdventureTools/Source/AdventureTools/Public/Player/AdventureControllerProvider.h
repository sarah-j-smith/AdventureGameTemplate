// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AdventureControllerProvider.generated.h"

class AAdventurePlayerController;
class AAdventureCharacter;


// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UAdventureControllerProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class ADVENTURETOOLS_API IAdventureControllerProvider
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UFUNCTION(BlueprintCallable, Category = "AdventureController")
    virtual AAdventurePlayerController *GetAdventurePlayerController();

    UFUNCTION(BlueprintCallable, Category = "AdventureController")
    virtual AAdventureCharacter *GetAdventureCharacter();
};
