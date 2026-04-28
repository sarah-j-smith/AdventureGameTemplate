// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DialogInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDialogInteractable : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class DIALOG_API IDialogInteractable
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    /// What to do when a conversation is started with this. 
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
    void OnConverseWith();
    
};
