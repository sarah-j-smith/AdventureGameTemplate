// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "VerbType.h"
#include "Enums/BarkAction.h"
#include "Enums/PlayerCommand.h"

#include "GameFramework/Actor.h"
#include "Player/AdventureControllerProvider.h"
#include "Player/BarkProvider.h"
#include "Player/PlayerBarkManager.h"

#include "CommandManager.generated.h"

namespace EPathFollowingResult
{
    enum Type : int;
}

class UAssetActionComponent;
class UAdventureGameInstance;
class UAdventureGameHUD;
class AAdventureAIController;
class APuck;
class UItemManager;
class AAdventureCharacter;
class UInteractionNotifier;
class UTestBarkController;

DECLARE_MULTICAST_DELEGATE(FUpdateInteractionText);
DECLARE_MULTICAST_DELEGATE(FBeginAction);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInterruptAction);

class UItemSlot;
class AHotSpot;

///
/// Delegate class used by the Player Controller to manage commands issued
/// by the player, and to track the current state of command interactions
/// such as walk to location, hotspot or use verb on item or hotspot
/// One of these should be dropped into each level.
UCLASS(BlueprintType, Blueprintable)
class ADVENTURETOOLS_API ACommandManager : public AActor,
        public IAdventureControllerProvider, public IBarkProvider
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ACommandManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    virtual void Destroyed() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    //////////////////////////////////
    ///
    /// INPUT LOCKING
    ///

    void SetInputLocked(bool bLocked);

    /// At certain times, eg level loading, do not allow any user input.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Getter="IsInputLocked", Category = UI)
    bool bInputLocked = false;

    bool IsInputLocked() const;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
    UInteractionNotifier* InteractionNotifier;

    bool ShouldHighlightInteractionText() const;

    //////////////////////////////////
    ///
    /// EVENT HANDLERS
    ///

    /// Tell the UI to put the current verb and any current hotspot into the text display
    /// and if the HotspotInteraction is true, highlight the text.
    FUpdateInteractionText UpdateInteractionTextDelegate;

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void AddInputHandlers(APuck* Puck);

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void HandleTouchInput();

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void HandlePointAndClickInput();

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void HandleHotSpotClicked(AHotSpot* HotSpot);

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void HandleLocationClicked(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void MouseEnterHotSpot(AHotSpot* HotSpot);

    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void MouseLeaveHotSpot();

    void UpdateMouseOverUI(bool NewMouseIsOverUI);

    /// Handle a mouse click on an item button.
    UFUNCTION(BlueprintCallable, Category = "EventHandlers")
    void HandleInventoryItemClicked(UItemSlot* ItemSlot);

    //////////////////////////////////
    ///
    /// PERFORM COMMANDS
    ///
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Actions")
    UAssetActionComponent* AssetActionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    UItemManager* ItemManager;

    void PerformInstantAction();

    void PerformHotSpotInteraction();

    void WalkToLocation(const FVector& Location);

    void WalkToHotSpot(AHotSpot* HotSpot);

    void CommenceConversation();

    void EndConversation();

    //////////////////////////////////
    ///
    /// INTERRUPT COMMANDS
    ///

    bool ShouldInterruptCurrentActionOnNextTick() const
    {
        if (const UPlayerBarkManager* APlayerBarkManager = GetBarkController())
        {
            return bShouldInterruptCurrentActionOnNextTick && APlayerBarkManager->IsPlayerBarking() ==
                EBarkAction::NotBarking;
        }
        return bShouldInterruptCurrentActionOnNextTick;
    }

    void TeleportToLocation(const FVector& Location);

    UFUNCTION(BlueprintCallable, Category="Actions")
    void AssignVerb(EVerbType NewVerb);

    /// Stops any current action, items and hotspots, clearing the status
    UFUNCTION(BlueprintCallable, Category="InterruptCommands", DisplayName="ClearAction")
    void InterruptCurrentAction();

    /// Call InterruptCurrentAction async, in the next tick
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="InterruptCommands")
    bool bShouldInterruptCurrentActionOnNextTick = false;

    FBeginAction BeginAction;

    /// Event fired when the current action is terminated/interrupted
    UPROPERTY(BlueprintAssignable, Category="InterruptCommands")
    FInterruptAction InterruptAction;

    //////////////////////////////////
    ///
    /// PLAYER CHARACTER AI
    ///

    /// Player character instance that this command manager will command.
    /// This value will be automatically filled with the player character present in
    /// the scene, unless it is already set in a blueprint. 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerCharacter")
    AAdventureCharacter* PlayerCharacter;

    enum class EAIStatus:uint8
    {
        Idle,
        MakingRequest,
        AlreadyThere,
        Moving,
        Done
    };

    EAIStatus AIStatus = EAIStatus::Idle;

    enum class EAIMoveResult
    {
        Unknown,
        Moving,
        Success,
        Fail
    };

    void ConnectToMoveCompletedDelegate();
    
    void DisconnectFromMoveCompletedDelegate();

    /**
     * Called by AI Controller to notify that pathing is finished. Can be immediately & synchronously
     * (ie before even processing any of the switch statement in the above function)
     * if the agent/player is already at the location. Or can be after moving there. Schedules
     * `HandleMovementComplete` to be called on the next timer tick.
     */
    UFUNCTION()
    void HandleAIMovementCompleteNotify(EPathFollowingResult::Type Result);

    bool IsAlreadyAtHotspotClicked() const
    {
        return AIStatus == EAIStatus::AlreadyThere;
    }

    bool ShouldCompleteMovementNextTick = false;

    UFUNCTION()
    void HandleMovementComplete();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement")
    FVector TargetLocationForAI = FVector::ZeroVector;

    void ClearCurrentPath();

    EAIMoveResult LastPathResult = EAIMoveResult::Unknown;

    void SetVerbAndCommandFromHotSpot(AHotSpot* HotSpot);

    void StopAIMovement();
    
    /// A switch to teleport to new locations in the game instead using AI to
    /// walk there over the nav graph. Can be used with non-AI / non-nav mesh
    /// movement like an empty or invisible character. Also useful for testing.
    /// In a game set this on the AdventureGameMode, as that setting will
    /// override this.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bTeleportInsteadOfWalk = false;

    //////////////////////////////////
    ///
    /// HUD MESSAGES
    ///
    ///

    /// Bind button handlers so if the verbs are clicked the command manager can
    /// track the current command and verb chosen
    /// @param AdventureGameHUD HUD that has the buttons to get messages from
    UFUNCTION(BlueprintCallable, Category = "Actions")
    void AddUIHandlers(UAdventureGameHUD* AdventureGameHUD);
    
    /// Bind update handlers so changes in the game state - such as inventory
    /// or score - update the HUD display
    /// @param AdventureGameHUD HUD that has the display to send messages to
    void ConnectToPlayerHUD(UAdventureGameHUD* AdventureGameHUD);
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    UAdventureGameHUD* AdventureGameHUD;

    UPROPERTY(EditAnywhere, Category = "HUD")
    TSubclassOf<UAdventureGameHUD> AdventureHUDClass;

    /// Create an AdventureGameHUD and attach it to the above property,
    /// then setup handlers for it. This should never be called in regular
    /// game play as the setup functions are done by the Player Controller.
    /// 
    /// But in tests (where there is no player controller) it might be useful
    /// to display the HUD to check the tests are working as expected.
    ///
    /// Generally showing UI during a test - they should test functionality.
    /// Tests should be able to run headless and just report "PASS" or "FAIL".
    /// This <code>SetupHUD()</code> will be called in the Command Manager's
    /// <code>BeginPlay</code> if the <code>bIsTesting && !bDisableHUD</code>
    /// boolean flags are set appropriately.
    UFUNCTION(BlueprintCallable, Category = "Testing")
    void SetupHUD();

    /// Returns the PlayerBarkManager, unless testing when the TestBarkController
    /// may be returned instead.
    UFUNCTION(BlueprintCallable, Category = "Player Bark")
    UPlayerBarkManager* GetBarkController() const;

private:
    UPROPERTY()
    UPlayerBarkManager* PlayerBarkManager;

    /// Track barks in testing
    UPROPERTY()
    UTestBarkController* TestBarkController;

public:
    //////////////////////////////////
    ///
    /// COMMAND STATE
    ///
    ///

    /// Is there currently player barks on the screen
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    bool bIsPlayerBarking = false;

    /// Is there currently NPC barks on the screen
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    bool bIsNPCBarking = false;

    /// Has the player currently issued a command?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    EPlayerCommand CurrentCommand = EPlayerCommand::None;

    /// Current verb action selected by the player
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    EVerbType CurrentVerb = EVerbType::WalkTo;

    /// Current hovered or clicked Hotspot
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    TObjectPtr<AHotSpot> CurrentHotSpot;

    /// Location that the player is being sent to by a click
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Commands")
    FVector CurrentTargetLocation = FVector::ZeroVector;


    //////////////////////////////////
    ///
    /// COMMAND LOGIC UTILITIES
    ///

    bool CanBrowseHotspot()
    {
        switch (CurrentCommand)
        {
        case EPlayerCommand::None:
        case EPlayerCommand::Hover:
        case EPlayerCommand::Targeting:
        case EPlayerCommand::UsePending:
        case EPlayerCommand::VerbPending:
            return true;
        default:
            return false;
        }
    }

    bool CanBrowseTarget()
    {
        return CurrentCommand == EPlayerCommand::Targeting;
    }

    bool CanBrowseSource()
    {
        switch (CurrentCommand)
        {
        case EPlayerCommand::None:
        case EPlayerCommand::Hover:
        case EPlayerCommand::VerbPending:
        case EPlayerCommand::UsePending:
        case EPlayerCommand::GivePending:
            return true;
        default:
            return false;
        }
    }

    //////////////////////////////////
    ///
    /// TESTING SWITCHES
    /// 

    /// Informational switch for use by blueprints
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestingOnly")
    bool bIsTesting = false;

    /// A switch to turn off player barking. Only useful for tests.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestingOnly")
    bool bDisablePlayerBarking = false;

    /// A switch to turn off forwarding of Enhanced Input events from the <code>APuck</code>
    /// instance to this controller. Only useful for tests.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TestingOnly")
    bool bDisableInputSystem = false;

    /// A switch to turn off loading of rooms on launch. Only useful for tests.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TestingOnly")
    bool bDisableRoomLoading = false;

    /// A switch to turn off referencing the HUD / UI. The UI creation is done by the
    /// AdventurePlayerController - use the Disable HUD switch there to prevent
    /// it being created.  If it is not created then set this to true to avoid
    /// uselessly trying to access it in the Command Manager.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestingOnly")
    bool bDisableHUD = false;

    /// A switch to turn off sending update display events to the HUD / UI.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestingOnly")
    bool bDisableHUDUpdates = false;

    /// If true use a stub to implement <code>Bark</code> and <code>BarkAndEnd</code>.
    /// The stub simply stores the requested barks for use in tests. If false try to
    /// use the real bark UI. Only useful in testing.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TestingOnly")
    bool bUseTestBarkController = false;

private:
    AAdventureCharacter* GetPlayerCharacter();

    AAdventureAIController* GetAIController();

    UAdventureGameInstance* GetAdventureGameInstance() const;
};
