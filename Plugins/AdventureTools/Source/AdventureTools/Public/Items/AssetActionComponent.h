// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "ItemManagerProvider.h"
#include "Player/BarkProvider.h"
#include "Components/ActorComponent.h"
#include "AssetActionComponent.generated.h"


enum class EItemAssetType : uint8;
class UItemDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTURETOOLS_API UAssetActionComponent : public UActorComponent, public IItemManagerProvider, public IBarkProvider
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UAssetActionComponent();
    
    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
    
    
    UFUNCTION(BlueprintCallable, Category="ItemAction")
    void StartTimer(UItemDataAsset *DataAsset);

    UFUNCTION(BlueprintCallable, Category="ItemAction")
    void StopTimer();

    /// Triggered when this item is the <b>target</b> of a use verb, and it successfully
    /// passes initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemUseSuccess(UItemDataAsset *DataAsset);

    /// Triggered when this item is the <b>target</b> of a give verb, and it successfully
    /// passes initial checks. The source item of the give verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemGiveSuccess(UItemDataAsset *DataAsset);

    /// Triggered when this item is the <b>target</b> of a use verb, and it successfully
    /// passes initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemUseFailure(UItemDataAsset *DataAsset);

    /// Triggered when this item is the <b>target</b> of a give verb, and it successfully
    /// passes initial checks. The source item of the give verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemGiveFailure(UItemDataAsset *DataAsset);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool TimerRunning = false;
	
    UFUNCTION()
    void OnInteractionTimeout();
	
    FTimerHandle ActionHighlightTimerHandle;
    
private:
    void HandleSourceItem(UItemDataAsset *DataAsset, EItemAssetType ItemAssetType, bool &Success);
    void HandleTargetItem(UItemDataAsset *DataAsset, EItemAssetType ItemAssetType, bool &Success);

    void HandleKeyCase(bool &Success);
};
