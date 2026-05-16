// (c) 2025 Sarah Smith

#include "Player/CommandManager.h"

#include "AdventureTools.h"

#include "HotSpots/HotSpot.h"
#include "Items/InventoryItem.h"
#include "Items/AssetActionComponent.h"
#include "HUD/AdventureGameHUD.h"
#include "HUD/VerbsUI.h"
#include "HUD/ItemSlot.h"
#include "Gameplay/AdventureGameModeBase.h"

#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/PawnMovementComponent.h"

#include "Player/AdventureAIController.h"
#include "Player/AdventurePlayerController.h"
#include "Player/AdventureCharacter.h"
#include "Player/InteractionNotifier.h"
#include "Player/ItemManager.h"
#include "Player/Puck.h"
#include "Player/TestBarkController.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

void ShowLocationDebug(float LocationX, float LocationY, const FString& LocationMessage)
{
#if WITH_EDITOR
    const FString Message = FString::Printf(TEXT("%s x: %f, y: %f"), *LocationMessage, LocationX, LocationY);
    GEngine->AddOnScreenDebugMessage(LOCATION_DEBUG_KEY, 5.0, FColor::Cyan,
                                     *Message, false, FVector2D(2.0, 2.0));
    UE_LOG(LogAdventureGame, Display, TEXT("%s"), *Message);
#endif
}

// Sets default values
ACommandManager::ACommandManager()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Construct: ACommandManager - %p"), this);

    InteractionNotifier = CreateDefaultSubobject<UInteractionNotifier>(TEXT("InteractionNotifier"));
    ItemManager = CreateDefaultSubobject<UItemManager>(TEXT("ItemManager"));
    PlayerBarkManager = CreateDefaultSubobject<UPlayerBarkManager>(TEXT("PlayerBark"));
    AssetActionComponent = CreateDefaultSubobject<UAssetActionComponent>("AssetActionComponent");
}

// Called when the game starts or when spawned
void ACommandManager::BeginPlay()
{
    Super::BeginPlay();
    
    SetupHUD();
    
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("BeginPlay: ACommandManager - %p"), this);
    if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
}

void ACommandManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("EndPlay: ACommandManager - %p"), this);
}

void ACommandManager::Destroyed()
{
    Super::Destroyed();
    
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Destroyed: ACommandManager - %p"), this);
}


// Called every frame
void ACommandManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ShouldInterruptCurrentActionOnNextTick())
    {
        InterruptCurrentAction();
        bShouldInterruptCurrentActionOnNextTick = false;
    }
    if (ShouldCompleteMovementNextTick)
    {
        HandleMovementComplete();
        ShouldCompleteMovementNextTick = false;
    }
}

void ACommandManager::SetInputLocked(bool bLocked)
{
    bInputLocked = bLocked;
}

bool ACommandManager::IsInputLocked() const
{
    return bInputLocked;
}

bool ACommandManager::ShouldHighlightInteractionText() const
{
    return CurrentCommand == EPlayerCommand::Active || CurrentCommand == EPlayerCommand::InstantActive;
}

void ACommandManager::HandleTouchInput()
{
    // Don't test input, start from HandleHotSpotClicked & HandleLocationClicked
    check(!bIsTesting); 
    InteractionNotifier->NotifyUserInteraction();

    if (IsInputLocked()) return;

    float LocationX, LocationY;
    AAdventurePlayerController* AdventurePlayerController = GetAdventurePlayerController();
    if (!AdventurePlayerController) return;
    if (!AdventurePlayerController->GetTouchPosition(LocationX, LocationY)) return;

    ShowLocationDebug(LocationX, LocationY, TEXT("Touch input"));

    if (AHotSpot* HotSpot = AdventurePlayerController->HotSpotTapped(LocationX, LocationY))
    {
        HandleHotSpotClicked(HotSpot);
    }
    else
    {
        FVector MouseWorldLocation, MouseWorldDirection;
        AdventurePlayerController->DeprojectScreenPositionToWorld(LocationX, LocationY, MouseWorldLocation,
                                                                  MouseWorldDirection);

        if (const AAdventureCharacter* APlayerCharacter = GetPlayerCharacter())
        {
            const FVector PlayerLocation = APlayerCharacter->GetCapsuleComponent()->GetComponentLocation();
            MouseWorldLocation.Z = PlayerLocation.Z;
        }
        HandleLocationClicked(MouseWorldLocation);
    }
}

void ACommandManager::HandlePointAndClickInput()
{
    // Don't test input, start from HandleHotSpotClicked & HandleLocationClicked
    check(!bIsTesting);
    
    InteractionNotifier->NotifyUserInteraction();

    AAdventurePlayerController* AdventurePlayerController = GetAdventurePlayerController();
    if (IsInputLocked() || !AdventurePlayerController) return;

    if (AHotSpot* HotSpot = AdventurePlayerController->HotSpotClicked())
    {
        SetVerbAndCommandFromHotSpot(HotSpot);
        HandleHotSpotClicked(HotSpot);
    }
    else
    {
        float LocationX, LocationY;
        if (!AdventurePlayerController->GetMouseClickPosition(LocationX, LocationY)) return;

        ShowLocationDebug(LocationX, LocationY, TEXT("Click input"));

        FVector MouseWorldLocation, MouseWorldDirection;
        AdventurePlayerController->DeprojectScreenPositionToWorld(LocationX, LocationY, MouseWorldLocation,
                                                                  MouseWorldDirection);

        if (const AAdventureCharacter* APlayerCharacter = GetPlayerCharacter())
        {
            const FVector PlayerLocation = APlayerCharacter->GetCapsuleComponent()->GetComponentLocation();
            MouseWorldLocation.Z = PlayerLocation.Z;
        }
        HandleLocationClicked(MouseWorldLocation);
    }
}

void ACommandManager::HandleHotSpotClicked(AHotSpot* HotSpot)
{
    if (!IsValid(HotSpot)) return;

#if WITH_EDITOR
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("HandleHotSpotClicked - %s - command: %s"),
           *HotSpot->ShortDescription.ToString(),
           *UEnum::GetValueAsString(CurrentCommand));
#endif

    switch (CurrentCommand)
    {
    case EPlayerCommand::InstantActive:
        InterruptCurrentAction();
    case EPlayerCommand::None:
    case EPlayerCommand::Hover:
        CurrentHotSpot = HotSpot;
        ItemManager->SourceItem = nullptr;
        ItemManager->TargetItem = nullptr;
        PerformInstantAction();
        break;
    case EPlayerCommand::VerbPending:
    case EPlayerCommand::UsePending:
        CurrentCommand = EPlayerCommand::Active;
        CurrentHotSpot = HotSpot;
        ItemManager->ClearSourceItem();
        ItemManager->ClearTargetItem();
        if (!bDisableHUDUpdates) BeginAction.Broadcast();
        WalkToHotSpot(HotSpot);
        break;
    case EPlayerCommand::Targeting:
        CurrentCommand = EPlayerCommand::Active;
        CurrentHotSpot = HotSpot;
        ItemManager->ClearTargetItem();
        if (!bDisableHUDUpdates) BeginAction.Broadcast();
        WalkToHotSpot(HotSpot);
        break;
    default:
        break;
    }
}

void ACommandManager::HandleLocationClicked(const FVector& Location)
{
#if WITH_EDITOR
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("HandleLocationClicked - %s - command: %s"), *Location.ToString(),
           *UEnum::GetValueAsString(CurrentCommand));
#endif
    CurrentTargetLocation = Location;
    switch (CurrentCommand)
    {
    case EPlayerCommand::InstantActive:
        InterruptCurrentAction();
    case EPlayerCommand::None:
    case EPlayerCommand::Hover:
        ItemManager->ClearSourceItem();
        ItemManager->ClearTargetItem();
        PerformInstantAction();
        break;
    default:
        break;
    }
}

void ACommandManager::MouseEnterHotSpot(AHotSpot* HotSpot)
{
    if (CanBrowseHotspot())
    {
        CurrentHotSpot = HotSpot;
        if (!bDisableHUDUpdates)
        {
            if (UpdateInteractionTextDelegate.IsBound())
            {
                UpdateInteractionTextDelegate.Broadcast();
                UE_LOG(LogAdventureGame, Display, TEXT("Broadcasting to UpdateInteractionTextDelegate"))
            }
        }
        else
        {
            UE_LOG(LogAdventureGame, VeryVerbose, TEXT("MouseEnterHotSpot but bDisableHUDUpdates set: ignoring"));
        }
    }
}

void ACommandManager::MouseLeaveHotSpot()
{
    if (CanBrowseHotspot() && CurrentHotSpot)
    {
        CurrentHotSpot = nullptr;
        if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
    }
}

void ACommandManager::PerformInstantAction()
{
#if WITH_EDITOR
    FString DebugString;
    if (ItemManager->SourceItem) DebugString = ItemManager->SourceItem->ShortDescription.ToString();
    if (CurrentHotSpot && DebugString.IsEmpty()) DebugString = CurrentHotSpot->ShortDescription.ToString();
    UE_LOG(LogAdventureGame, Display, TEXT("PerformInstantAction %s - %s"),
           *VerbGetDescriptiveString(CurrentVerb).ToString(), *DebugString);
#endif

    CurrentCommand = EPlayerCommand::InstantActive;
    if (ItemManager->SourceItem)
    {
        // Clicking on something in your own inventory
        UInventoryItem::Execute_OnLookAt(ItemManager->SourceItem);
        ItemManager->UpdateInventoryText();
    }
    else if (CurrentHotSpot)
    {
        WalkToHotSpot(CurrentHotSpot);
        if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
    }
    else
    {
        WalkToLocation(CurrentTargetLocation);
        if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
    }
    if (!bDisableHUDUpdates) BeginAction.Broadcast();
}

void ACommandManager::PerformHotSpotInteraction()
{
    UE_LOG(LogAdventureGame, Verbose, TEXT("PerformHotSpotInteraction - verb %s for hotspot %s"),
           *VerbGetDescriptiveString(CurrentVerb).ToString(), *CurrentHotSpot->ShortDescription.ToString());
    // This `Execute_Verb` pattern will call C++ and Blueprint overrides.
    // The use of eg CurrentHotSpot->OnClose() does not work as BP's don't do
    // polymorphism and have to be dispatched in code.
    check(CurrentHotSpot);
    switch (CurrentVerb)
    {
    case EVerbType::Close:
        AHotSpot::Execute_OnClose(CurrentHotSpot);
        break;
    case EVerbType::Open:
        AHotSpot::Execute_OnOpen(CurrentHotSpot);
        break;
    case EVerbType::Give:
        AHotSpot::Execute_OnGive(CurrentHotSpot);
        break;
    case EVerbType::GiveItem:
        AHotSpot::Execute_OnItemGiven(CurrentHotSpot);
        break;
    case EVerbType::LookAt:
        AHotSpot::Execute_OnLookAt(CurrentHotSpot);
        break;
    case EVerbType::PickUp:
        AHotSpot::Execute_OnPickUp(CurrentHotSpot);
        break;
    case EVerbType::TalkTo:
        AHotSpot::Execute_OnTalkTo(CurrentHotSpot);
        break;
    case EVerbType::Pull:
        AHotSpot::Execute_OnPull(CurrentHotSpot);
        break;
    case EVerbType::Push:
        AHotSpot::Execute_OnPush(CurrentHotSpot);
        break;
    case EVerbType::Use:
        AHotSpot::Execute_OnUse(CurrentHotSpot);
        break;
    case EVerbType::UseItem:
        AHotSpot::Execute_OnItemUsed(CurrentHotSpot);
        break;
    case EVerbType::WalkTo:
        AHotSpot::Execute_OnWalkTo(CurrentHotSpot);
    default:
        break;
    }
}

void ACommandManager::ConnectToMoveCompletedDelegate()
{
    if (AAdventureAIController *AdventureAIController = GetAIController())
    {
        AdventureAIController->MoveCompletedDelegate.AddDynamic(
            this, &ACommandManager::HandleAIMovementCompleteNotify);
    }
}

void ACommandManager::DisconnectFromMoveCompletedDelegate()
{
    if (AAdventureAIController *AdventureAIController = GetAIController())
    {
        AdventureAIController->MoveCompletedDelegate.RemoveDynamic(this, &ACommandManager::HandleAIMovementCompleteNotify);
    }
}

void ACommandManager::HandleAIMovementCompleteNotify(EPathFollowingResult::Type Result)
{
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("HandleAIMovementCompleteNotify"));
    if (Result == EPathFollowingResult::Success)
    {
        if (AIStatus == EAIStatus::MakingRequest)
        {
            AIStatus = EAIStatus::AlreadyThere;
        }
        LastPathResult = EAIMoveResult::Success;
        ShouldCompleteMovementNextTick = true;
    }
    else
    {
        if (CurrentHotSpot && (Result == EPathFollowingResult::Blocked || Result == EPathFollowingResult::OffPath))
        {
            FVector HotSpotLocation = CurrentHotSpot->WalkToPoint->GetComponentLocation();
#if WITH_EDITOR
            const FString Message = FString::Printf(
                TEXT("Movement blocked to %s - %s - is walk to point on the nav mesh?"),
                *(CurrentHotSpot->ShortDescription.ToString()), *(HotSpotLocation.ToString()));
            GEngine->AddOnScreenDebugMessage(1, 5.0, FColor::Cyan,
                                             *Message, false, FVector2D(2.0, 2.0));
            UE_LOG(LogAdventureGame, Warning, TEXT("%s"), *Message);
#endif
        }
    }
}

void ACommandManager::HandleMovementComplete()
{
    AAdventureCharacter* APlayerCharacter = GetPlayerCharacter();
    check(APlayerCharacter);
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("HandleMovementComplete"));
    AIStatus = EAIStatus::Idle;
    if (CurrentHotSpot && LastPathResult == EAIMoveResult::Success)
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("CurrentHotSpot && (LastPathResult == EAIMoveResult::Success)"));
        APlayerCharacter->SetFacingDirection(CurrentHotSpot->FacingDirection);
        APlayerCharacter->TeleportToLocation(CurrentHotSpot->WalkToPosition);
        PerformHotSpotInteraction();
        return;
    } else if (!TargetLocationForAI.IsZero() && LastPathResult == EAIMoveResult::Success)
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("TargetLocationForAI && (LastPathResult == EAIMoveResult::Success)"));
        APlayerCharacter->TeleportToLocation(TargetLocationForAI);
    }
    InterruptCurrentAction();
}

void ACommandManager::WalkToLocation(const FVector& Location)
{
    StopAIMovement();
    if (AIStatus != EAIStatus::Idle) return;
    TargetLocationForAI = Location;
    
    if (bTeleportInsteadOfWalk)
    {
        TeleportToLocation(Location);
        return;
    }

    if (AAdventureAIController* AI = GetAIController())
    {
        AIStatus = EAIStatus::MakingRequest;
        switch (AI->MoveToLocation(Location, 1.0))
        {
        case EPathFollowingRequestResult::Type::Failed:
            UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Path following request -> failed: %f %f"), Location.X, Location.Y);
            LastPathResult = EAIMoveResult::Fail;
            break;
        case EPathFollowingRequestResult::Type::RequestSuccessful:
            UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Path following request -> success: %f %f"), Location.X, Location.Y);
            LastPathResult = EAIMoveResult::Moving;
            AIStatus = EAIStatus::Moving;
            break;
        case EPathFollowingRequestResult::Type::AlreadyAtGoal:
            UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Path following request -> already there: %f %f"), Location.X,
                   Location.Y);
            break;
        }
    }
}

void ACommandManager::WalkToHotSpot(AHotSpot* HotSpot)
{
    const AAdventureCharacter* APlayerCharacter = GetPlayerCharacter();
    const UCapsuleComponent* Capsule = APlayerCharacter->GetCapsuleComponent();
    const FVector PlayerLocation = Capsule->GetComponentLocation();

    FVector HotSpotWalkToLocation = HotSpot->WalkToPosition;
    HotSpotWalkToLocation.Z = PlayerLocation.Z;

    CurrentHotSpot = nullptr;
    float Distance = FVector::Distance(HotSpotWalkToLocation, PlayerLocation);
    if (Distance < Capsule->GetScaledCapsuleRadius())
    {
        // Character is there already, or very close to. Teleport to location and carry on.
        TeleportToLocation(HotSpotWalkToLocation);
        CurrentHotSpot = HotSpot;
        return;
    }
    WalkToLocation(HotSpotWalkToLocation);
    switch (AIStatus)
    {
    case EAIStatus::Moving:
    case EAIStatus::AlreadyThere:
        // Don't set the hotspot unless we know the player can reach it.
        CurrentHotSpot = HotSpot;
    default:
        break;
    }
}

void ACommandManager::CommenceConversation()
{
    SetInputLocked(true);
    InteractionNotifier->PromptListOpenRequest.Broadcast();
}

void ACommandManager::EndConversation()
{
    SetInputLocked(false);
    InteractionNotifier->PromptListCloseRequest.Broadcast();
}

void ACommandManager::AssignVerb(EVerbType NewVerb)
{
    ItemManager->ClearSourceItem();
    ItemManager->ClearTargetItem();
    CurrentVerb = NewVerb;
    switch (NewVerb)
    {
    case EVerbType::Use:
        CurrentCommand = EPlayerCommand::UsePending;
        break;
    case EVerbType::Give:
        CurrentCommand = EPlayerCommand::GivePending;
        break;
    case EVerbType::WalkTo:
        break;
    default:
        CurrentCommand = EPlayerCommand::VerbPending;
    }
    if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
}

void ACommandManager::InterruptCurrentAction()
{
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("InterruptCurrentAction"));
    SetInputLocked(false);
    TargetLocationForAI = FVector::ZeroVector;
    if (AAdventureCharacter* APlayerCharacter = GetPlayerCharacter())
    {
        APlayerCharacter->GetMovementComponent()->StopActiveMovement();
    }
    CurrentVerb = EVerbType::WalkTo;
    CurrentCommand = EPlayerCommand::None;
    CurrentHotSpot = nullptr;
    ItemManager->Reset();
    if (!bDisableHUDUpdates) InterruptAction.Broadcast();
}

void ACommandManager::ClearCurrentPath()
{
    if (AAdventureAIController* AIController = GetAIController())
    {
        AIController->StopMovement();
    }
}

void ACommandManager::TeleportToLocation(const FVector& Location)
{
    AAdventureCharacter* APlayerCharacter = GetPlayerCharacter();
    FVector Dest = Location;
    Dest.Z = APlayerCharacter->GetCapsuleComponent()->GetComponentLocation().Z;
    APlayerCharacter->TeleportToLocation(Dest);
    LastPathResult = EAIMoveResult::Success;
    AIStatus = EAIStatus::AlreadyThere;
    ShouldCompleteMovementNextTick = true;
}

void ACommandManager::SetVerbAndCommandFromHotSpot(AHotSpot* HotSpot)
{
    // If the player has not selected a verb, but has clicked on a hotspot, polymorphically
    // check with the hotspot for a default command that the player might expect, such as "Look"
    // or "Open" for a closed door, or "Use" for an open door, or "Talk to" for an NPC.
    if (CurrentCommand != EPlayerCommand::None && CurrentCommand != EPlayerCommand::Hover) return;

    // Player clicked on a hotspot without specifying a verb first.
    if (CurrentVerb != EVerbType::WalkTo)
    {
        // If CurrentCommand is None/Hover then the verb _should be_ the default
        UE_LOG(LogAdventureGame, Warning, TEXT("HotSpot %s clicked with no command but verb %s unexpectedly set!!"),
               *HotSpot->ShortDescription.ToString(), *VerbGetDescriptiveString(CurrentVerb).ToString());
    }
    CurrentVerb = HotSpot->CheckForDefaultCommand();
    if (CurrentVerb != EVerbType::WalkTo)
    {
        CurrentCommand = CurrentVerb == EVerbType::Use ? EPlayerCommand::UsePending : EPlayerCommand::VerbPending;
    }
}

void ACommandManager::StopAIMovement()
{
    AAdventureCharacter* APlayerCharacter = GetPlayerCharacter();
    if (!IsValid(APlayerCharacter)) return;
    AAdventureAIController* AI = Cast<AAdventureAIController>(APlayerCharacter->GetController());
    if (!IsValid(AI))
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("PlayerCharacter controller expected to be AIController"));
        return;
    }
    if (AIStatus == EAIStatus::Moving)
    {
        AI->StopMovement();
        APlayerCharacter->GetMovementComponent()->StopActiveMovement();
        AIStatus = EAIStatus::Idle;
    }
}

void ACommandManager::ConnectToPlayerHUD(UAdventureGameHUD* AAdventureGameHUD)
{
    if (bDisableHUDUpdates) return;

    AAdventureGameHUD->BindNotifierHandlers(InteractionNotifier);
    AAdventureGameHUD->BindCommandHandlers(this);

    if (UAdventureGameInstance *AdventureGameInstance = GetAdventureGameInstance())
    {
        AAdventureGameHUD->BindInventoryHandlers(AdventureGameInstance);
    }
    if (AAdventureGameModeBase *GameMode = Cast<AAdventureGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        AAdventureGameHUD->BindScoreHandlers(GameMode);
    }
}

void ACommandManager::AddUIHandlers(UAdventureGameHUD *AAdventureGameHUD)
{
    if (!bDisableHUD) AAdventureGameHUD->VerbsUI->OnVerbChanged.BindDynamic(this, &ACommandManager::AssignVerb);
}

void ACommandManager::SetupHUD()
{
    if (bDisableHUD) return;

    // Create the HUD and put it on the screen
    APlayerController *PlayerController = UGameplayStatics::GetPlayerController(this, 0);
    AdventureGameHUD = UAdventureGameHUD::Create(PlayerController, AdventureHUDClass);
    AdventureGameHUD->ShowBlackScreen(); // Hide the game while its being loaded
    
    // Send button presses from the HUD to the Command Manager
    AddUIHandlers(AdventureGameHUD);

    // Send player commands & game state changes to the HUD
    ConnectToPlayerHUD(AdventureGameHUD);

    if (!bDisablePlayerBarking) PlayerBarkManager->SetAdventureGameHUD(AdventureGameHUD);
    if (bUseTestBarkController)
    {
        PlayerBarkManager->DestroyComponent();
        PlayerBarkManager = nullptr;
        TestBarkController = CreateDefaultSubobject<UTestBarkController>(TEXT("TestBarkController"));
    }
    if (PlayerBarkManager && IsValid(PlayerBarkManager))
    {
        PlayerBarkManager->SetAdventureGameHUD(AdventureGameHUD);
        if (bIsTesting) AdventureGameHUD->Bark->OverrideDisplayTime = 1.0f;
    }
}

UPlayerBarkManager* ACommandManager::GetBarkController() const
{
    if (TestBarkController) return TestBarkController;
    return PlayerBarkManager;
}

AAdventureCharacter* ACommandManager::GetPlayerCharacter()
{
    if (PlayerCharacter) return PlayerCharacter;
    if (bIsTesting)
    {
        ///   In testing we drag a CommandManager and an AdventureCharacter into the test level, and
        ///   hook them up manually so that the this->PlayerCharacter is set already. 
        UE_LOG(LogAdventureGame, Error, TEXT("PlayerCharacter is not set during testing!"));
        return nullptr;
    }
    return GetAdventureCharacter();
}

AAdventureAIController* ACommandManager::GetAIController()
{
    /// IMPORTANT NOTE:
    ///   In normal game play the AdventurePlayerController (APC) will drive the setup of these
    ///   handlers in its BeginPlay() function. But in functional tests the APC does not exist.
    if (const AAdventureCharacter* APlayerCharacter = GetPlayerCharacter())
    {
        AAdventureAIController* AI = Cast<AAdventureAIController>(APlayerCharacter->GetController());
        if (!AI)
        {
            UE_LOG(LogAdventureGame, Warning, TEXT("Player character expected to have AI controller - got %s"),
                *(AI == nullptr ? TEXT("NULL") : AI->GetName()));
        }
        return AI;
    }
    return nullptr;
}

UAdventureGameInstance* ACommandManager::GetAdventureGameInstance() const
{
    if (UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(
    UGameplayStatics::GetGameInstance(this)))
    {
        return AdventureGameInstance;
    }
    UE_LOG(LogAdventureGame, Warning, TEXT("Could not get Adventure Game Instance"));
    return nullptr;
}

void ACommandManager::UpdateMouseOverUI(const bool NewMouseIsOverUI)
{
    if (NewMouseIsOverUI)
    {
        if (CurrentVerb == EVerbType::WalkTo && CurrentCommand == EPlayerCommand::None)
        {
            CurrentVerb = EVerbType::LookAt;
            if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
        }
    }
    else
    {
        if (CurrentVerb == EVerbType::LookAt && CurrentCommand == EPlayerCommand::None)
        {
            CurrentVerb = EVerbType::WalkTo;
            if (!bDisableHUDUpdates) UpdateInteractionTextDelegate.Broadcast();
        }
    }
}

void ACommandManager::AddInputHandlers(APuck* Puck)
{
    if (bDisableInputSystem) return;
    Puck->PointAndClickDelegate.AddUObject(this, &ACommandManager::HandlePointAndClickInput);
    Puck->TouchInputDelegate.AddUObject(this, &ACommandManager::HandleTouchInput);
}

void ACommandManager::HandleInventoryItemClicked(UItemSlot* ItemSlot)
{
    if (!ItemSlot)
    {
        UE_LOG(LogAdventureGame, Error, TEXT("HandleInventoryItemClicked: Item slot is NULL"));
        return;
    }
    if (!ItemSlot->HasItem)
    {
        // clicking an empty inventory slot clears everything out
        InterruptCurrentAction();
        return;
    }
    if (!ItemManager->MaybeHandleInventoryItemClicked(ItemSlot)) return;

    // This handler is only called if `HasItem` is true
    switch (CurrentCommand)
    {
    case EPlayerCommand::None:
    case EPlayerCommand::Hover:
        ItemManager->SetAndLockSourceItem(ItemSlot->InventoryItem);
        PerformInstantAction();
        break;
    case EPlayerCommand::VerbPending:
        ItemManager->SetAndLockSourceItem(ItemSlot->InventoryItem);
        CurrentCommand = EPlayerCommand::Active;
        ItemManager->PerformItemAction(CurrentVerb);
        if (!bDisableHUDUpdates) BeginAction.Broadcast();
        break;
    case EPlayerCommand::UsePending:
    case EPlayerCommand::GivePending:
        ItemManager->SetAndLockSourceItem(ItemSlot->InventoryItem);
        CurrentVerb = CurrentCommand == EPlayerCommand::GivePending ? EVerbType::GiveItem : EVerbType::UseItem;
        CurrentCommand = EPlayerCommand::Targeting;
        ItemManager->ClearTargetItem(); // Should be clear already
        ItemManager->UpdateInventoryText();
        break;
    case EPlayerCommand::Targeting:
        if (ItemManager->CanInteractWith(ItemSlot->InventoryItem->ItemKind))
        {
            ItemManager->SetAndLockTargetItem(ItemSlot->InventoryItem);
            CurrentCommand = EPlayerCommand::Active;
            ItemManager->PerformItemInteraction(CurrentVerb);
            if (!bDisableHUDUpdates) BeginAction.Broadcast();
        }
    default:
        UE_LOG(LogAdventureGame, Display, TEXT("Ignoring inventory click on %s - command is: %s"),
            *(ItemSlot->InventoryItem->ShortDescription.ToString()),
            *UEnum::GetValueAsString(CurrentCommand));
    }
}
