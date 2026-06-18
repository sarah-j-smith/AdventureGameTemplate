// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GetItemSlotTask.generated.h"

class UInventoryUI;
class UAdventureGameHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGetSlotSuccessOutputPin, UItemSlot *, Item);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGetSlotFailOutputPin);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UGetItemSlotTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable)
    FGetSlotSuccessOutputPin TaskSuccessful;

    UPROPERTY(BlueprintAssignable)
    FGetSlotFailOutputPin TaskFailed;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"),
        Category = "Flow Control")
    
    static UGetItemSlotTask* DoGetItemSlotTask(const UObject* WorldContextObject, UAdventureGameHUD *HUD, 
        FName ItemKind, float WaitTime = 20.0f);

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
    void OnInventoryUIChanged();
    
    TWeakObjectPtr<UInventoryUI> InventoryHUD;

    bool CheckForSuccessCondition(const UInventoryUI* InventoryUI);
};
