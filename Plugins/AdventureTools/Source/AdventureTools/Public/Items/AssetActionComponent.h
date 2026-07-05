// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "AssetActionComponent.generated.h"


class UManagerProvider;
enum class EItemAssetType : uint8;
class UStoryAction;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTURETOOLS_API UAssetActionComponent : public UActorComponent
{
    GENERATED_BODY()

    UPROPERTY()
    UManagerProvider *ManagerProvider;
    
public:
    // Sets default values for this component's properties
    UAssetActionComponent();
    
    // Called when the game starts
    virtual void BeginPlay() override;
    
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
    
    UFUNCTION(BlueprintCallable, Category="ItemAction")
    void StartTimer(UStoryAction *DataAsset);

    UFUNCTION(BlueprintCallable, Category="ItemAction")
    void StopTimer();

    /// Triggered when this item is the <b>target</b> of a use verb, and it successfully
    /// passes initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemActionSuccess(UStoryAction *DataAsset);

    /// Triggered when this item is the <b>target</b> of a use verb, and it successfully
    /// passes initial checks. The source item of the use verb is found by querying
    /// the adventure player controller's <code>SourceItem</code> property.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemHandling")
    void OnItemActionFailure(UStoryAction *DataAsset);
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    bool TimerRunning = false;
	
    UFUNCTION()
    void OnInteractionTimeout();
	
    FTimerHandle ActionHighlightTimerHandle;
    
private:
    void HandleSourceItem(UStoryAction *DataAsset, EItemAssetType ItemAssetType, bool &Success);
    void HandleTargetItem(UStoryAction *DataAsset, EItemAssetType ItemAssetType, bool &Success);

    void HandleKeyCase(bool &Success);
    void HandlePickupCase(bool &Success, UStoryAction *DataAsset);
};
