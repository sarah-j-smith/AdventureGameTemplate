// (c) 2025 Sarah Smith


#include "Gameplay/AdventureGameInstance.h"

#include "Gameplay/AdventureSave.h"
#include "AdventureTools.h"
#include "GameUtils.h"
#include "Constants.h"
#include "HotSpots/HotSpot.h"
#include "HotSpots/Door.h"
#include "Items/ItemList.h"
#include "Items/InventoryItem.h"
#include "HUD/AdventureGameHUD.h"
#include "Player/CommandManager.h"

#include "GameFramework/SaveGame.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Player/AdventureCharacter.h"

void UAdventureGameInstance::Init()
{
	Super::Init();

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

UInventoryItem* UAdventureGameInstance::AddItemToInventory(EItemKind ItemKind)
{
	if (Inventory)
	{
		return Inventory->AddItemToInventory(ItemKind);
	}
	return nullptr;
}

void UAdventureGameInstance::RemoveItemFromInventory(EItemKind ItemKind)
{
	if (Inventory)
	{
		Inventory->RemoveItemKindFromInventory(ItemKind);
	}
}

void UAdventureGameInstance::RemoveItemsFromInventory(const TSet<EItemKind>& ItemsToRemove)
{
	if (Inventory)
	{
		Inventory->RemoveItemKindsFromInventory(ItemsToRemove);
	}
}


bool UAdventureGameInstance::IsInInventory(const EItemKind& ItemToCheck) const
{
	return (Inventory && Inventory->Contains(ItemToCheck));
}

UInventoryItem* UAdventureGameInstance::GetItemFromInventory(const EItemKind& ItemToCheck)
{
	if (Inventory && Inventory->Contains(ItemToCheck))
	{
		TArray<UInventoryItem*> Items;
		Inventory->GetInventoryItemsArray(Items);
		for (UInventoryItem* Item : Items)
		{
			if (Item->ItemKind == ItemToCheck)
			{
				return Item;
			}
		}
	}
	return nullptr;
}

void UAdventureGameInstance::GetInventoryItems(TArray<UInventoryItem*>& Items)
{
	if (Inventory)
	{
		Inventory->GetInventoryItemsArray(Items);
	}
}

int UAdventureGameInstance::GetInventoryItemCount() const
{
	return Inventory ? Inventory->InventorySize : 0;
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
		NewRoomDelay();
		break;
	case ERoomTransitionPhase::LoadNewRoom:
		UE_LOG(LogAdventureGame, Log, TEXT("UAdventureGameInstance::OnRoomLoaded - LoadNewRoom"));
		RoomTransitionPhase = ERoomTransitionPhase::NewRoomLoaded;
		UnloadRoom();
	default:
		UE_LOG(LogAdventureGame, Warning, TEXT("Unexpected state during OnRoomLoaded"));
		break;
	}
}

void UAdventureGameInstance::NewRoomDelay()
{
	RoomTransitionPhase = ERoomTransitionPhase::DelayProcessing;

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
	const ADoor* Door = FindDoor(CurrentDoorLabel);
	LoadDoor(Door);
	CurrentDoor = const_cast<ADoor*>(Door);

	if (UAdventureGameHUD* HUD = GetHUD())
	{
		HUD->HideBlackScreen();
	}

	if (ACommandManager* Command = GetCommandManager())
	{
		Command->InterruptCurrentAction();
	}
}

void UAdventureGameInstance::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GameplayTags);
}

void UAdventureGameInstance::StartNewRoom()
{
	if (ACommandManager* Command = GetCommandManager())
	{
		Command->SetInputLocked(false);
	}
	RoomTransitionPhase = ERoomTransitionPhase::RoomCurrent;
}

void UAdventureGameInstance::OnRoomUnloaded()
{
	NewRoomDelay();
}

void UAdventureGameInstance::TriggerRoomTransition()
{
	RoomTransitionPhase = ERoomTransitionPhase::RoomCurrent;
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

	TArray<UInventoryItem*> Items;
	CurrentSaveGame->Inventory.Reset(Inventory->InventorySize);
	Inventory->GetInventoryItemsArray(Items);
	for (const UInventoryItem* Item : Items)
	{
		CurrentSaveGame->Inventory.Add(Item->ItemKind);
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
	for (const EItemKind Item : CurrentSaveGame->Inventory)
	{
		Inventory->AddItemToInventory(Item);
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
	RoomTransitionPhase = ERoomTransitionPhase::LoadNewRoom;
	if (ACommandManager* Command = GetCommandManager())
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

void UAdventureGameInstance::LoadDoor(const ADoor* Door)
{
	if (!Door)
	{
		return;
	}

	UE_LOG(LogAdventureGame, Display, TEXT("UAdventureGameInstance::LoadDoor: %s"),
	       *(Cast<ADoor>(Door)->ShortDescription.ToString()));

	if (AAdventureCharacter* AdventureCharacter = GetAdventureCharacter())
	{
		FVector Location = Door->WalkToPoint->GetComponentLocation();
		Location.Z = AdventureCharacter->GetCapsuleComponent()->GetComponentLocation().Z;
		AdventureCharacter->TeleportToLocation(Location);
		AdventureCharacter->SetFacingDirection(Door->FacingDirection);
		AdventureCharacter->SetupCamera();
	}
}

void UAdventureGameInstance::LogSaveGameStatus(USaveGame* SaveGame)
{
}

void UAdventureGameInstance::CreateInventory()
{
	if (!Inventory)
	{
		if (InventoryClass)
		{
			Inventory = NewObject<UItemList>(this, InventoryClass, TEXT(PLAYER_INVENTORY_NAME));
		}
		else
		{
			Inventory = NewObject<UItemList>(this, TEXT(PLAYER_INVENTORY_NAME));
			UE_LOG(LogAdventureGame, Log,
			       TEXT(
				       "Created new inventory of UItemList type. Set InventoryClass property in AdventureGameInstance to customise this."
			       ));
		}
		Inventory->Identifier = PLAYER_INVENTORY_NAME;
	}
}

void UAdventureGameInstance::DestroyInventory()
{
	Inventory->OnInventoryChanged.Remove(OnInventoryChangedHandle);
	Inventory = nullptr;
}

void UAdventureGameInstance::BindInventoryChangedHandlers()
{
	if (!Inventory->OnInventoryChanged.IsBound())
	{
		OnInventoryChangedHandle = Inventory->OnInventoryChanged.AddUObject(
			this, &UAdventureGameInstance::InventoryChanged);
	}
}

void UAdventureGameInstance::InventoryChanged(FName InventoryIdentifier, EItemKind ItemKind,
                                              EItemDisposition ItemDisposition)
{
	if (InventoryIdentifier == PLAYER_INVENTORY_NAME)
	{
		PlayerInventoryChanged.Broadcast(ItemKind, ItemDisposition);
	}
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
