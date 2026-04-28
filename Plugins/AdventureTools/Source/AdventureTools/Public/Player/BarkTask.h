// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemManagerProvider.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "BarkTask.generated.h"

/// https://unrealcommunity.wiki/creating-asynchronous-blueprint-nodes-ctnmtj0q

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBarkOutputPin);

/**
 * Manage player asynchronous bark from blueprint actions. Use this when you want the player to bark
 * a message and wait until the bark is finished before continuing with more blueprint nodes/actions.
 */
UCLASS()
class ADVENTURETOOLS_API UBarkTask : public UBlueprintAsyncActionBase, public IItemManagerProvider
{
    GENERATED_UCLASS_BODY()
public:
    /// The bark was displayed and after its allotted time was removed from the screen.
    UPROPERTY(BlueprintAssignable)
    FBarkOutputPin TaskCompleted;

    /// The bark was displayed and then a player click or tap dismissed it.
    UPROPERTY(BlueprintAssignable)
    FBarkOutputPin TaskInterrupted;

    /**
     * Async task for the player-character to bark some text and then pause
     * for a time so the player can read it.
     * @param WorldContextObject Supplied by the blueprint, ignore.
     * @param BarkText Text to bark. Enter "\n" line feeds to split into rows.
     * @return The created bark task will either end successfully or be interrupted. Use the appropriate pins to respond to each.
     */
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "Flow Control")
    static UBarkTask* DoBarkTask(const UObject* WorldContextObject, const FText BarkText);

    virtual void Activate() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Task Control")
    const UObject* WorldContextObject;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Task Control")
    int32 MyUID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Barking")
    FText BarkText;

private:
    UFUNCTION()
    void BarkCompleted(int32 UID, EBarkRequestFinishedReason Reason);
};
