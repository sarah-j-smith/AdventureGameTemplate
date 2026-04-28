// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BarkProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UBarkProvider : public UInterface
{
    GENERATED_BODY()
};

/**
 * 
 */
class ADVENTURETOOLS_API IBarkProvider
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    
    /// Clear any current bark, and immediately bark a message.
    /// Player barks a message and then ends any action sequence
    /// they were doing, unblocked & allowing user input again.
    /// Use when the blueprint event logic should end with a bark.
    /// @param BarkText FText for the player to bark. Should be translatable.
    UFUNCTION(BlueprintCallable, Category = "PlayerBark")
    virtual void BarkAndEnd(FText BarkText);

    /// Player barks a message and continues on any action sequence
    /// they were doing. Use when the event logic should continue
    /// and, user interaction should remain blocked.
    ///
    /// Queues the bark to happen after any current barks. 
    /// @param BarkText FText for the player to bark. Should be translatable.
    UFUNCTION(BlueprintCallable, Category = "PlayerBark")
    virtual void Bark(FText BarkText);

    /// Player barks a series of messages and continues on any action sequence
    /// they were doing. Use when the event logic should continue
    /// and, user interaction should remain blocked.
    ///
    /// Queues the bark to happen after any current barks. 
    UFUNCTION(BlueprintCallable, Category = "PlayerBark")
    virtual void BarkLines(TArray<FText> BarkTextArray);

    /// Clear any current bark messages being displayed, including
    /// all queued messages.
    UFUNCTION(BlueprintCallable, Category = "PlayerBark")
    virtual void ClearBark();
};
