// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "BarkRequestFinishReason.h"
#include "Constants.h"
#include "Components/ActorComponent.h"

#include "PlayerBarkManager.generated.h"

enum class EBarkAction : uint8;
class ACommandManager;
class UAdventureGameHUD;

DECLARE_MULTICAST_DELEGATE_TwoParams(FEndBark, int32 /* UID */, EBarkRequestFinishedReason /* BarkFinishReason */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTURETOOLS_API UPlayerBarkManager : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UPlayerBarkManager();

    //////////////////////////////////
    ///
    /// PLAYER BARK
    ///

    /// Do a bark and immediately end the player actions
    virtual void PlayerBarkAndEnd(const FText &BarkText);
	
    /// Bark but potentially have more actions to follow
    virtual void PlayerBark(const FText &BarkText, int32 BarkTaskUid = BARK_UID_NONE);

    /// Bark a number of lines and potentially have more actions to follow
    virtual void PlayerBarkLines(const TArray<FText> &BarkTextArray, int32 BarkTaskUid = BARK_UID_NONE);

    /// Immediately end the player actions
    virtual void ClearBark();

    /// This flag is true when the Bark Timer is running, and false otherwise.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barking")
    bool IsBarking = false;

    UFUNCTION(BlueprintCallable, Category="Barking", BlueprintPure)
    EBarkAction IsPlayerBarking() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barking")
    TArray<int32> CurrentBarkTasks;
    
    TSet<int32> BarkTasksShouldCausePlayerInterrupt;
    
    FEndBark EndPlayerBark;

    void SetAdventureGameHUD(class UAdventureGameHUD *HUD);

    void OnTimeoutBark(int32 BarkTaskId);
    void OnInterruptBark(int32 BarkTaskId);

private:
    UPROPERTY()
    UAdventureGameHUD *AdventureHUDWidget;

public:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
