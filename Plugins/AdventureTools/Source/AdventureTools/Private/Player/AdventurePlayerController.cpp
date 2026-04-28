// (c) 2025 Sarah Smith


#include "Player/AdventurePlayerController.h"

#include "Player/AdventureAIController.h"
#include "Player/AdventureCharacter.h"
#include "Player/Puck.h"

#include "Gameplay/AdventureSave.h"
#include "HotSpots/HotSpot.h"
#include "GameUtils.h"
#include "AdventureTools.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AAdventurePlayerController::AAdventurePlayerController()
{
    SetShowMouseCursor(true);
    DefaultMouseCursor = EMouseCursor::Crosshairs;
    bEnableMouseOverEvents = true;
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Construct: AAdventurePlayerController"));
}

void AAdventurePlayerController::BeginPlay()
{
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT(">>>> BeginPlay: AAdventurePlayerController"));
    Super::BeginPlay();

    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("     Super done - BeginPlay: AAdventurePlayerController - calling GetPawn"));

    APawn* PlayerPawn = GetPawn();
    PlayerCharacter = Cast<AAdventureCharacter>(PlayerPawn);
    check(PlayerCharacter);
    SetupAnimationDelegates();

    /// Set up commands and the HUD
    SetupCommandManager();
    
    Puck = SetupPuck(PlayerCharacter);
    SetupAIController(PlayerCharacter);
    Command->ConnectToMoveCompletedDelegate();

    SaveGameToSlotDelegate.BindUObject(this, &AAdventurePlayerController::OnSaveGameComplete);
    LoadGameFromSlotDelegate.BindUObject(this, &AAdventurePlayerController::OnLoadGameComplete);

    if (UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        /// Load the starting room
        AdventureGameInstance->OnLoadRoom();
    }
    
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("<<<< BeginPlay: AAdventurePlayerController"));
}

void AAdventurePlayerController::OnSaveGameComplete(const FString& SlotName, const int32 UserIndex, bool Success)
{
    Command->SetInputLocked(false);
    UpdateSaveGameIndicator.Broadcast(ESaveGameStatus::Saved, Success);
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("SaveGame: Saved - %s"), Success ? TEXT("true") : TEXT("false"));
}

void AAdventurePlayerController::HandleSaveGame(const FString& GameName)
{
    UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(GetGameInstance());
    if (!AdventureGameInstance) return;
    
    Command->SetInputLocked(true);
    UpdateSaveGameIndicator.Broadcast(ESaveGameStatus::Saving, true);
    AdventureGameInstance->SaveGame();
    // Writes out all non-transient properties - so a UPROPERTY that does not have the Transient flag
    UGameplayStatics::AsyncSaveGameToSlot(AdventureGameInstance->CurrentSaveGame, GameName, 0, SaveGameToSlotDelegate);
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("SaveGame: %s Save commenced"), *GameName);
}

void AAdventurePlayerController::HandleLoadGame(const FString& GameName)
{
    UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(GetGameInstance());
    if (!AdventureGameInstance) return;

    Command->SetInputLocked(true);
    UpdateSaveGameIndicator.Broadcast(ESaveGameStatus::Loading, false);
    UGameplayStatics::AsyncLoadGameFromSlot(GameName, 0, LoadGameFromSlotDelegate);
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("LoadGame: %s Load commenced"), *GameName);
}

void AAdventurePlayerController::OnLoadGameComplete(const FString& SlotName, const int32, USaveGame* LoadedGame)
{
    UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(GetGameInstance());
    checkf(AdventureGameInstance != nullptr, TEXT("AdventureGameInstance was nullptr"));
    checkf(LoadedGame != nullptr, TEXT("Loaded game is null"));
    UAdventureSave* AdventureSave = Cast<UAdventureSave>(LoadedGame);
    checkf(AdventureSave != nullptr, TEXT("Loaded game wrong save class"));
    if (AdventureSave == nullptr)
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("LoadGame: %s failed, or could not be cast to UAdventureSave"),
               *SlotName);
        UpdateSaveGameIndicator.Broadcast(ESaveGameStatus::Loaded, false);
    }
    else
    {
        AdventureGameInstance->CurrentSaveGame = AdventureSave;
        AdventureGameInstance->LoadGame();
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("LoadGame: %s Loaded"), *SlotName);
        UpdateSaveGameIndicator.Broadcast(ESaveGameStatus::Loaded, true);
    }
    Command->SetInputLocked(false);
}

void AAdventurePlayerController::SetupAnimationDelegates()
{
    PlayerCharacter->AnimationCompleteDelegate.AddDynamic(this, &AAdventurePlayerController::OnPlayerAnimationComplete);
}

void AAdventurePlayerController::SetupCommandManager()
{
    AActor* Actor = UGameplayStatics::GetActorOfClass(GetWorld(), ACommandManager::StaticClass());
    if (ACommandManager* CommandManager = Cast<ACommandManager>(Actor))
    {
        UE_LOG(LogAdventureGame, Display, TEXT("Found ACommandManager in scene, using it"));
        Command = CommandManager;
    }
    else
    {
        UE_LOG(LogAdventureGame, Display, TEXT("Spawning %s - none found in scene"), *CommandManagerToSpawn->GetName());
        Actor = GetWorld()->SpawnActor(CommandManagerToSpawn);
        Command = Cast<ACommandManager>(Actor);
    }
}

APawn* AAdventurePlayerController::SetupPuck(AAdventureCharacter* APlayerCharacter) const
{
    check(APlayerCharacter);
    UCapsuleComponent* CapsuleComp = APlayerCharacter->GetCapsuleComponent();
    check(CapsuleComp);
    const FVector CapsuleLocation = CapsuleComp->GetComponentLocation();
    const FVector FSpawnLocation(CapsuleLocation.X, 0.0, 0.0);

    check(PuckClassToSpawn)
    APuck* NewPuck = GetWorld()->SpawnActor<APuck>(
        PuckClassToSpawn,
        FSpawnLocation,
        FRotator::ZeroRotator);

    Command->AddInputHandlers(NewPuck);
    return NewPuck;
}

void AAdventurePlayerController::SetupAIController(APawn* AttachToPawn)
{
    UE_LOG(LogAdventureGame, VeryVerbose, TEXT("       AAdventurePlayerController::SetupAIController"));
    AActor* AIControllerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AAdventureAIController::StaticClass());
    AAdventureAIController* AdventureAIController = Cast<AAdventureAIController>(AIControllerActor);
#if WITH_EDITOR
    if (!AdventureAIController)
    {
        FString ErrorMessage = FString::Printf(TEXT("AIController not found on character %s - check its blueprint"),
            *AttachToPawn->GetName());
        GEngine->AddOnScreenDebugMessage(1, 10.0, FColor::Red,
        *ErrorMessage,false, FVector2D(2.0, 2.0));
        UE_LOG(LogAdventureGame, Error, TEXT("SetupAIController error. %s"), *ErrorMessage);
    }
#endif
    check(AdventureAIController);

    AdventureAIController->Possess(AttachToPawn);

    // DO NOT CONNECT THE COMMAND MANAGER HERE.
    // During functional testing the Player Controller is not available, and we need
    // the Command Manager (an AActor) to discover the AI Controller itself.
}

bool AAdventurePlayerController::GetMouseClickPosition(float& LocationX, float& LocationY)
{
    float NewLocationX = 0.0f;
    float NewLocationY = 0.0f;
    if (!GetMousePosition(NewLocationX, NewLocationY)) return false;
    const FVector2D ThisMouseClick{NewLocationX, NewLocationY};
    if (!FGameUtils::HasChangedMuch(ThisMouseClick, LastMouseClick)) return false;
    LastMouseClick = ThisMouseClick;
    LocationX = NewLocationX;
    LocationY = NewLocationY;
    return true;
}

bool AAdventurePlayerController::GetTouchPosition(float& LocationX, float& LocationY)
{
    bool bIsPressed;
    float NewLocationX = 0.0f;
    float NewLocationY = 0.0f;
    GetInputTouchState(ETouchIndex::Type::Touch1, NewLocationX, NewLocationY, bIsPressed);
    if (!bIsPressed) return false;
    const FVector2D ThisMouseClick{NewLocationX, NewLocationY};
    if (!FGameUtils::HasChangedMuch(ThisMouseClick, LastMouseClick)) return false;
    LastMouseClick = ThisMouseClick;
    LocationX = NewLocationX;
    LocationY = NewLocationY;
    return true;
}

void AAdventurePlayerController::EndTaskAction(EInteractionType InteractionType, int32 UID, bool Complete)
{
    UE_LOG(LogAdventureGame, Warning, TEXT("AAdventurePlayerController::EndTaskAction - %s"),
           *(InteractionGetDescriptiveString(InteractionType)));
    if (InteractionType != EInteractionType::None)
    {
        EndAction.Broadcast(InteractionType, UID, Complete);
    }
    PlayerInteractUID = 0;
}

AHotSpot* AAdventurePlayerController::HotSpotClicked()
{
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, true, HitResult);
    if (!HitResult.IsValidBlockingHit()) return nullptr;
    AActor* HitActor = HitResult.GetActor();
    if (AHotSpot* HotSpot = Cast<AHotSpot>(HitActor))
    {
#if WITH_EDITOR
        FString HotSpotMessage = FString::Printf(TEXT("Got HotSpot: %s"), *HotSpot->GetName());
        // GEngine->AddOnScreenDebugMessage(1, 20.0, FColor::White, HotSpotMessage,
        //                                  false, FVector2D(2.0, 2.0));
        UE_LOG(LogAdventureGame, Display, TEXT("%s"), *HotSpotMessage);
#endif
        return HotSpot;
    }
    return nullptr;
}

AHotSpot* AAdventurePlayerController::HotSpotTapped(float X, float Y)
{
    FHitResult HitResult;
    GetHitResultUnderFinger(ETouchIndex::Type::Touch1, ECC_Visibility, true, HitResult);
    if (!HitResult.IsValidBlockingHit()) return nullptr;
    AActor* HitActor = HitResult.GetActor();
    if (AHotSpot* HotSpot = Cast<AHotSpot>(HitActor))
    {
        UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Got HotSpot: %s"), *HotSpot->GetName());
        return HotSpot;
    }
    return nullptr;
}

void AAdventurePlayerController::PlayerClimb(int32 UID, EInteractTimeDirection InteractDirection)
{
    PlayerClimbUID = UID;
    if (PlayerCharacter->LastNonZeroMovement.X != 0)
    {
        PlayerCharacter->Climb();
    }
    else
    {
        OnPlayerClimbComplete(false);
        UE_LOG(LogAdventureGame, Warning, TEXT("PlayerClimb called when player not facing left or right."))
    }
}

void AAdventurePlayerController::PlayerInteract(int32 UID, EInteractTimeDirection InteractDirection)
{
    PlayerInteractUID = UID;
    if (PlayerCharacter->LastNonZeroMovement.X != 0)
    {
        PlayerCharacter->Interact();
    }
    else
    {
        OnPlayerInteractComplete(false);
        UE_LOG(LogAdventureGame, Warning, TEXT("PlayerInteract called when player not facing left or right."))
    }
}

void AAdventurePlayerController::PlayerSit(int32 UID, EInteractTimeDirection InteractDirection)
{
    UE_LOG(LogAdventureGame, Warning, TEXT("AAdventurePlayerController::PlayerSit"));
    PlayerSitUID = UID;
    if (PlayerCharacter->LastNonZeroMovement.X != 0)
    {
        UE_LOG(LogAdventureGame, Warning, TEXT("     >>> PlayerSit"));
        PlayerCharacter->Sit();
    }
    else
    {
        OnPlayerSitComplete(false);
        UE_LOG(LogAdventureGame, Warning, TEXT("PlayerSit called when player not facing left or right."))
    }
}

void AAdventurePlayerController::PlayerTurnLeft(int32 UID, EInteractTimeDirection InteractDirection)
{
    PlayerTurnUID = UID;
    EWalkDirection Facing = PlayerCharacter->GetFacingDirection();
    switch (Facing)
    {
    case EWalkDirection::Up:
        UE_LOG(LogAdventureGame, VeryVerbose,
               TEXT("Current character does not distinguish front from back, in turn animation"));
    case EWalkDirection::Down:
        if (InteractDirection == EInteractTimeDirection::Forward)
        {
            // Turn from the front/back to the left
            PlayerCharacter->TurnLeft(InteractDirection);
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning, TEXT("PlayerTurnLeft in Reverse called when player not facing left"))
            OnPlayerTurnLeftComplete(false);
        }
        break;
    case EWalkDirection::Left:
        if (InteractDirection == EInteractTimeDirection::Backward)
        {
            // Turn from the left back to the front
            PlayerCharacter->TurnLeft(InteractDirection);
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning,
                   TEXT("PlayerTurnLeft called when player already facing left. Use backward?"))
            OnPlayerTurnLeftComplete(false);
        }
        break;
    case EWalkDirection::Right:
        UE_LOG(LogAdventureGame, Warning, TEXT("PlayerTurnLeft called when player facing right."))
        OnPlayerTurnLeftComplete(false);
        break;
    }
}

void AAdventurePlayerController::PlayerTurnRight(int32 UID, EInteractTimeDirection InteractDirection)
{
    UE_LOG(LogAdventureGame, Warning, TEXT("AAdventurePlayerController::PlayerTurnRight"));

    PlayerTurnUID = UID;
    EWalkDirection Facing = PlayerCharacter->GetFacingDirection();
    switch (Facing)
    {
    case EWalkDirection::Up:
        UE_LOG(LogAdventureGame, VeryVerbose,
               TEXT("Current character does not distinguish front from back, in turn animation"));
    case EWalkDirection::Down:
        if (InteractDirection == EInteractTimeDirection::Forward)
        {
            // Turn from the front/back to the left
            PlayerCharacter->TurnRight(InteractDirection);
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning, TEXT("PlayerTurnRight in Reverse called when player not facing left"))
            OnPlayerTurnRightComplete(false);
        }
        break;
    case EWalkDirection::Right:
        if (InteractDirection == EInteractTimeDirection::Backward)
        {
            // Turn from the left back to the front
            PlayerCharacter->TurnRight(InteractDirection);
        }
        else
        {
            UE_LOG(LogAdventureGame, Warning,
                   TEXT("PlayerTurnRight called when player already facing right. Use backward?"))
            OnPlayerTurnRightComplete(false);
        }
        break;
    case EWalkDirection::Left:
        UE_LOG(LogAdventureGame, Warning, TEXT("PlayerTurnRight called when player facing left."))
        OnPlayerTurnRightComplete(false);
        break;
    }
}

void AAdventurePlayerController::OnPlayerAnimationComplete(EInteractionType Interaction, bool Complete)
{
    switch (Interaction)
    {
    case EInteractionType::None:
        EndTaskAction(Interaction, 0, true);
        break;
    case EInteractionType::Interact:
        OnPlayerInteractComplete(Complete);
        break;
    case EInteractionType::Sit:
        OnPlayerSitComplete(Complete);
        break;
    case EInteractionType::Climb:
        OnPlayerClimbComplete(Complete);
        break;
    case EInteractionType::TurnLeft:
        OnPlayerTurnLeftComplete(Complete);
        break;
    case EInteractionType::TurnRight:
        OnPlayerTurnRightComplete(Complete);
        break;
    default: 
        UE_LOG(LogAdventureGame, Warning, TEXT("Unhandled animation completed %s"), 
            *UEnum::GetValueAsString(Interaction));
    }
}

void AAdventurePlayerController::OnPlayerInteractComplete(bool Complete)
{
    EndTaskAction(EInteractionType::Interact, PlayerInteractUID, Complete);
}

void AAdventurePlayerController::OnPlayerSitComplete(bool Complete)
{
    EndTaskAction(EInteractionType::Sit, PlayerSitUID, Complete);
}

void AAdventurePlayerController::OnPlayerClimbComplete(bool Complete)
{
    EndTaskAction(EInteractionType::Climb, PlayerClimbUID, Complete);
}

void AAdventurePlayerController::OnPlayerTurnLeftComplete(bool Complete)
{
    EndTaskAction(EInteractionType::TurnLeft, PlayerTurnUID, Complete);
}

void AAdventurePlayerController::OnPlayerTurnRightComplete(bool Complete)
{
    EndTaskAction(EInteractionType::TurnRight, PlayerTurnUID, Complete);
}
