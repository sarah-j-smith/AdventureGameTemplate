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

    virtual void PlayerBarkAndEnd(const FText &BarkText);
	
    virtual void PlayerBark(const FText &BarkText, int32 BarkTaskUid = BARK_UID_NONE);

    virtual void PlayerBarkLines(const TArray<FText> &BarkTextArray, int32 BarkTaskUid = BARK_UID_NONE);

    virtual void ClearBark();

    /// This flag is true when the Bark Timer is running, and false otherwise.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barking")
    bool IsBarking = false;

    UFUNCTION(BlueprintCallable, Category="Barking", BlueprintPure)
    EBarkAction IsPlayerBarking() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Barking")
    TArray<int32> CurrentBarkTasks;
    
    FEndBark EndPlayerBark;

    void SetAdventureGameHUD(class UAdventureGameHUD *HUD);

    void OnTimeoutBark(int32 BarkTaskId);
    void OnInterruptBark(int32 BarkTaskId);

private:
    UPROPERTY()
    UAdventureGameHUD *AdventureHUDWidget;

    ACommandManager *GetCommandManager();

public:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction) override;
};
