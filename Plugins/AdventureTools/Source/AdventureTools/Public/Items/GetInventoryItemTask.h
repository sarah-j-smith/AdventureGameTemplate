// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "GetInventoryItemTask.generated.h"

enum class EItemDisposition : uint8;
class UAdventureGameInstance;
class UInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetItemSuccessOutputPin, UItem *, Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetItemFailOutputPin);

/**
 * Wait a configurable amount of time for an item to appear in the players inventory.
 * Mostly useful for testing, but possibly useful in game as well.
 */
UCLASS()
class ADVENTURETOOLS_API UGetInventoryItemTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FGetItemSuccessOutputPin TaskSuccessful;

    UPROPERTY(BlueprintAssignable)
    FGetItemFailOutputPin TaskFailed;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "Flow Control")
    static UGetInventoryItemTask* DoGetInventoryItemTask(const UObject* WorldContextObject, FName ItemKind, float WaitTime = 20.0f);

    virtual void Activate() override;
    
    UPROPERTY()
    const UObject* WorldContextObject;

    FName ItemKind;

    float WaitTime;

private:
    FTimerHandle WaitTimer;
    bool TimerIsRunning = false;

    void StartWaitTimer();
    
    UFUNCTION()
    void WaitTimerTimeout();

    UFUNCTION()
    void OnPlayerInventoryChanged(FName ItemKind, EItemDisposition Disposition);

    UAdventureGameInstance *GetAdventureGameInstance();
    TWeakObjectPtr<UAdventureGameInstance> AdventureGameInstance;

    bool CheckForSuccessCondition(UAdventureGameInstance *GameInstance);
};
