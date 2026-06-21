// (c) 2025 Sarah Smith


#include "Gameplay/AdventureGameInstance.h"

#include "Gameplay/AdventureSave.h"
#include "AdventureTools.h"
#include "GameUtils.h"
#include "Constants.h"
#include "HotSpots/HotSpot.h"
#include "HotSpots/Door.h"
#include "HUD/AdventureGameHUD.h"
#include "Player/CommandManager.h"
#include "Inventory.h"

#include "GameFramework/SaveGame.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Gameplay/ManagerProvider.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AdventureCharacter.h"

void UAdventureGameInstance::Init()
{
	Super::Init();

	ManagerProvider = NewObject<UManagerProvider>(this);
	CreateInventory();
	BindInventoryChangedHandlers();

	if (ShouldCheckForSaveGameOnLoad && UGameplayStatics::DoesSaveGameExist(SAVE_GAME_NAME, 0))
	{
		if (USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SAVE_GAME_NAME, 0))
		{
			CurrentSaveGame = Cast<UAdventureSave>(SaveGame);
			LoadGame();
		}
	}
}

void UAdventureGameInstance::OnSaveHotSpot(AHotSpot* HotSpot)
{
	FDataSaveRecord SaveRecord;
	SaveRecord.LevelName = CurrentLevelName.ToString();
	SaveRecord.ObjectName = HotSpot->GetName();
	SaveRecord.Tags = HotSpot->GetTags();
	bool RecordFound = false;
	for (FDataSaveRecord& Record : AdventureSaves)
	{
		if (Record.LevelName == SaveRecord.LevelName && Record.ObjectName == SaveRecord.ObjectName)
		{
			Record.Tags = SaveRecord.Tags;
			RecordFound = true;
			break;
		}
	}
	if (!RecordFound)
	{
		AdventureSaves.Add(SaveRecord);
	}
}

void UAdventureGameInstance::OnLoadHotSpot(AHotSpot* HotSpot)
{
	for (const FDataSaveRecord& Record : AdventureSaves)
	{
		if (Record.LevelName == CurrentLevelName && Record.ObjectName == HotSpot->GetName())
		{
			HotSpot->SetTags(Record.Tags);
			break;
		}
	}
}

void UAdventureGameInstance::AddItemToInventory(FName ItemKind)
{
	if (Inventory)
	{
		Inventory->AddItemInstanceByName(ItemKind);
	}
}

void UAdventureGameInstance::RemoveItemFromInventory(FName ItemKind)
{
	if (Inventory)
	{
		Inventory->RemoveItemInstanceByName(ItemKind);
	}
}

void UAdventureGameInstance::RemoveItemsFromInventory(const TSet<FName>& ItemsToRemove)
{
	if (Inventory)
	{
		Inventory->RemoveItemKindsFromInventory(ItemsToRemove);
	}
}


bool UAdventureGameInstance::IsInInventory(const FName& ItemToCheck) const
{
	return (Inventory && Inventory->Contains(ItemToCheck));
}

UItem* UAdventureGameInstance::GetItemFromInventory(const FName& ItemToCheck)
{
	if (Inventory)
	{
		return Inventory->FindItemByName(ItemToCheck);
	}
	return nullptr;
}

void UAdventureGameInstance::GetInventoryItems(TArray<UItem*>& Items)
{
	if (Inventory)
	{
		Inventory->GetInventoryItemsArray(Items);
	}
}

int UAdventureGameInstance::GetInventoryItemCount() const
{
	return Inventory ? Inventory->GetInventorySize() : 0;
}

void UAdventureGameInstance::OnLoadRoom()
{
	UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::OnLoadRoom - RoomTransitionPhase: %s"),
	       *(UEnum::GetValueAsString(RoomTransitionPhase)));
	if (RoomTransitionPhase == ERoomTransitionPhase::GameNotStarted)
	{
		LoadStartingRoom();
	}
	else if (RoomTransitionPhase == ERoomTransitionPhase::RoomCurrent)
	{
		LoadRoom();
	}
}

void UAdventureGameInstance::LoadStartingRoom()
{
	// Load the room whose level name is in StartingLevelName, then call OnRoomLoaded.
	// This is done before there is a scene, or a player controller. Since there is no
	// scene we don't have to unload that previous level. 
	UE_LOG(LogAdventureGame, Log, TEXT("UAdventureGameInstance::LoadStartingRoom - %s"),
	       *StartingLevelName.ToString());
	RoomTransitionPhase = ERoomTransitionPhase::LoadStartingRoom;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);

	FLatentActionInfo LatentActionInfo = GetLatentActionForHandler(OnRoomLoadedName);
	UGameplayStatics::LoadStreamLevel(this, StartingLevelName,
	                                  true, false, LatentActionInfo);
}

void UAdventureGameInstance::OnRoomLoaded()
{
	switch (RoomTransitionPhase)
	{
	case ERoomTransitionPhase::LoadStartingRoom:
		UE_LOG(LogAdventureGame, Log, TEXT("UAdventureGameInstance::OnRoomLoaded - LoadStartingRoom"));
		CurrentLevelName = StartingLevelName;
		CurrentDoorLabel = StartingDoorLabel;
		RoomTransitionPhase = ERoomTransitionPhase::NewRoomLoaded;
		RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
		NewRoomDelay();
		break;
	case ERoomTransitionPhase::LoadNewRoom:
		UE_LOG(LogAdventureGame, Log, TEXT("UAdventureGameInstance::OnRoomLoaded - LoadNewRoom"));
		RoomTransitionPhase = ERoomTransitionPhase::NewRoomLoaded;
		RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
		UnloadRoom();
	default:
		UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected state during OnRoomLoaded"));
		break;
	}
}

void UAdventureGameInstance::NewRoomDelay()
{
	RoomTransitionPhase = ERoomTransitionPhase::DelayProcessing;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
	GetWorld()->GetTimerManager().SetTimer(
		RoomLoadTimer, this,
		&UAdventureGameInstance::OnRoomLoadTimerTimeout, RoomLoadDelay, false);
}

void UAdventureGameInstance::OnRoomLoadTimerTimeout()
{
	SetupRoom();
}

void UAdventureGameInstance::SetupRoom()
{
	auto f = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::SetupRoom - %s"), *f);

	UE_LOG(LogAdventureGame, Display, TEXT("Look for door %s"), *CurrentDoorLabel.ToString());

	// Find the door with the label.
	CurrentDoor = FindDoor(CurrentDoorLabel);

	if (ACommandManager* Command = ManagerProvider->GetCommandManager(this))
	{
		Command->InterruptCurrentAction();
	}
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		StartNewRoom();
	});
}

void UAdventureGameInstance::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

void UAdventureGameInstance::StartNewRoom()
{
	RoomTransitionPhase = ERoomTransitionPhase::RoomCurrent;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
	if (UAdventureGameHUD* HUD = GetHUD())
	{
		HUD->HideBlackScreen();
	}
	if (ACommandManager* Command = ManagerProvider->GetCommandManager(this))
	{
		Command->SetInputLocked(false);
	}
}

void UAdventureGameInstance::OnRoomUnloaded()
{
	NewRoomDelay();
}

void UAdventureGameInstance::TriggerRoomTransition()
{
	RoomTransitionPhase = ERoomTransitionPhase::RoomCurrent;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
	OnLoadRoom();
}

void UAdventureGameInstance::SaveGame()
{
	if (!IsValid(CurrentSaveGame))
	{
		CurrentSaveGame = Cast<UAdventureSave>(UGameplayStatics::CreateSaveGameObject(SaveGameClass));
	}

	CurrentSaveGame->StartingLevel = CurrentDoor->CurrentLevel;
	CurrentSaveGame->StartingDoorLabel = CurrentDoor->DoorLabel;

	TArray<UItem*> Items;
	CurrentSaveGame->Inventory.Reset(Inventory->GetInventorySize());
	Inventory->GetInventoryItemsArray(Items);
	for (const UItem* Item : Items)
	{
		CurrentSaveGame->Inventory.Add(Item->ItemTypeDef);
	}

	CurrentSaveGame->AdventureTags = GameplayTags;

	CurrentSaveGame->AdventureSaves.Empty();
	CurrentSaveGame->AdventureSaves.Append(AdventureSaves);

	CurrentSaveGame->OnAdventureSave(this);
}

void UAdventureGameInstance::LoadGame()
{
	if (!IsValid(CurrentSaveGame))
	{
		return;
	}

	if (RoomTransitionPhase == ERoomTransitionPhase::GameNotStarted)
	{
		StartingDoorLabel = CurrentSaveGame->StartingDoorLabel;
		StartingLevelName = CurrentSaveGame->StartingLevel;

		UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::LoadGame - GameNotStarted"));
	}
	else if (CurrentSaveGame->StartingLevel != CurrentLevelName)
	{
		SetLoadTarget(CurrentSaveGame->StartingLevel, CurrentSaveGame->StartingDoorLabel);
	}

	DestroyInventory();
	CreateInventory();
	for (const FName Item : CurrentSaveGame->Inventory)
	{
		Inventory->AddItemInstanceByName(Item);
	}
	BindInventoryChangedHandlers();

	GameplayTags = CurrentSaveGame->AdventureTags;

	AdventureSaves.Empty();
	AdventureSaves.Append(CurrentSaveGame->AdventureSaves);

	CurrentSaveGame->OnAdventureLoad(this);
}

void UAdventureGameInstance::RegisterHotSpotForSaveAndLoad(AHotSpot* HotSpot)
{
	HotSpot->DataLoad.BindDynamic(this, &UAdventureGameInstance::OnLoadHotSpot);
	RegisteredHotSpots.Add(HotSpot);
}

void UAdventureGameInstance::LoadRoom()
{
	// Load the room whose level name is in CurrentLevelName, then call OnRoomLoaded.
	// This is done when there is a scene, and a player controller, we must blank the screen,
	// stop player input, and unload that previous level (that unload is done in OnRoomLoaded). 
	// Note that the order is:
	//    * Load new room
	//    * Unload old room
	// So there is a brief period where two rooms are loaded at the same time. This is how level
	// streaming is supposed to work.
	RoomTransitionPhase = ERoomTransitionPhase::LoadNewRoom;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
	if (ACommandManager* Command = ManagerProvider->GetCommandManager(this))
	{
		Command->SetInputLocked(true);
		Command->InterruptCurrentAction();
	}
	GetHUD()->ShowBlackScreen();

	UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::LoadRoom - %s"), *CurrentLevelName.ToString());

	FLatentActionInfo LatentActionInfo = GetLatentActionForHandler(OnRoomLoadedName);
	UGameplayStatics::LoadStreamLevel(GetWorld(), CurrentLevelName,
	                                  true, false, LatentActionInfo);
}

void UAdventureGameInstance::UnloadRoom()
{
	if (Inventory)
	{
		if (UAdventureGameHUD* Hud = GetHUD())
		{
			Inventory->OnInventoryChanged.Remove(OnInventoryChangedHandle);
		}
	}
	RoomTransitionPhase = ERoomTransitionPhase::UnloadOldRoom;
	RoomTransitionedDelegate.Broadcast(RoomTransitionPhase);
	const FLatentActionInfo LatentActionInfo = GetLatentActionForHandler(OnRoomUnloadedName);
	UGameplayStatics::UnloadStreamLevel(GetWorld(), CurrentDoor->CurrentLevel, LatentActionInfo, false);
}

FLatentActionInfo UAdventureGameInstance::GetLatentActionForHandler(FName EventName)
{
	FLatentActionInfo LatentActionInfo;
	LatentActionInfo.Linkage = 0;
	LatentActionInfo.CallbackTarget = this;
	LatentActionInfo.ExecutionFunction = EventName;
	LatentActionInfo.UUID = FGameUtils::GetUUID();
	return LatentActionInfo;
}

/// TriggerRoomTransition to a room based on a door that has a new level name and door label
void UAdventureGameInstance::SetDestinationFromDoor(ADoor* NewCurrentDoor)
{
	CurrentDoor = NewCurrentDoor;
	CurrentDoorLabel = CurrentDoor->DoorLabel;
	CurrentLevelName = CurrentDoor->LevelToLoad;
	TriggerRoomTransition();
}

/// TriggerRoomTransition to a room based on the level name and door label
void UAdventureGameInstance::SetLoadTarget(FName LevelName, FName DoorLabel)
{
	UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::SetLoadTarget - Level: %s, Door: %s"),
	       *(LevelName.ToString()), *(DoorLabel.ToString()));
	CurrentDoorLabel = DoorLabel;
	CurrentLevelName = LevelName;
	TriggerRoomTransition();
}

ADoor* UAdventureGameInstance::FindDoor(FName DoorLabel)
{
	TArray<AActor*> Doors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADoor::StaticClass(), Doors);
	if (AActor** FoundDoor = Doors.FindByPredicate([DoorLabel](auto Element)
	{
		return Cast<ADoor>(Element)->DoorLabel == DoorLabel;
	}))
	{
#if WITH_EDITOR
		UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::FindDoor - got: %s"),
		       *(Cast<ADoor>(*FoundDoor)->ShortDescription.ToString()));
#endif
		return Cast<ADoor>(*FoundDoor);
	}
	UE_LOG(LogAdventureGame, Error, TEXT("UAdventureGameInstance::FindDoor failed to find %s"),
	       *(DoorLabel.ToString()));
	return nullptr;
}

void UAdventureGameInstance::LogSaveGameStatus(USaveGame* SaveGame)
{
}

void UAdventureGameInstance::CreateInventory()
{
	if (!Inventory)
	{
		Inventory = NewObject<UInventory>(this, TEXT(PLAYER_INVENTORY_NAME));
		Inventory->SetupHandlers();
		UE_LOG(LogAdventureGame, Log, TEXT("Created new inventory."));
		Inventory->Identifier = PLAYER_INVENTORY_NAME;
	}
}

void UAdventureGameInstance::DestroyInventory()
{
	Inventory->OnInventoryChanged.Remove(OnInventoryChangedHandle);
	Inventory->TearDownHandlers();
	Inventory = nullptr;
}

void UAdventureGameInstance::BindInventoryChangedHandlers()
{
	if (!Inventory->OnInventoryChanged.IsBound())
	{
		OnInventoryChangedHandle = Inventory->OnInventoryChanged.AddUObject(this, &UAdventureGameInstance::InventoryChanged);
	}
}

void UAdventureGameInstance::InventoryChanged(FName ItemKind,
                                              EItemDisposition ItemDisposition)
{
	PlayerInventoryChanged.Broadcast(ItemKind, ItemDisposition);
}

void UAdventureGameInstance::LoadRoom(ADoor* FromDoor)
{
	UGameInstance* GameInstance = FromDoor->GetGameInstance();
	UAdventureGameInstance* AdventureGameInstance = Cast<UAdventureGameInstance>(GameInstance);
	AdventureGameInstance->SetDestinationFromDoor(FromDoor);
}

UAdventureGameHUD* UAdventureGameInstance::GetHUD()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UAdventureGameHUD::StaticClass(), true);
	return FoundWidgets.IsEmpty() ? nullptr : Cast<UAdventureGameHUD>(FoundWidgets[0]);
}
