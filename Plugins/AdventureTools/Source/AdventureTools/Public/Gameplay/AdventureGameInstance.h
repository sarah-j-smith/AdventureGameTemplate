// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DataSaveRecord.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"

#include "Engine/TimerHandle.h"

#include "Engine/GameInstance.h"
#include "Enums/RoomTransitionPhase.h"
#include "Items/InventoryItem.h"
#include "Kismet/GameplayStatics.h"

#include "AdventureGameInstance.generated.h"

class UItemTypeDefs;
class UManagerProvider;
class UItem;
class UInventory;
class UInventoryItem;
class AHotSpot;
class UItemList;
class UAdventureSave;
class ADoor;
class UAdventureGameHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerInventoryChanged, FName, ItemKind, 
	EItemDisposition, ItemDisposition);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRoomTransitioned, 
	ERoomTransitionPhase, RoomTransitionPhase);

#define PLAYER_INVENTORY_NAME "PlayerInventory"

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UAdventureGameInstance : public UGameInstance, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
public:
	//////////////////////////////////
	///
	/// EVENT HANDLERS
	///

	virtual void Init() override;

	UFUNCTION()
	void OnSaveHotSpot(AHotSpot* HotSpot);

	UFUNCTION()
	void OnLoadHotSpot(AHotSpot* HotSpot);
	
	//////////////////////////////////
	///
	/// INVENTORY
	///

	/// Custom inventory item behaviours. This table is optional but must be set to a new Data Table,
	/// with FItemRow as the row type, if it is used. The entries in the table map _names_ of 
	/// items in the game to sub-classes (usually Blueprints) of <code>UInventoryItem</code> which 
	/// can have a Blueprint script attached to provide custom behaviours. 
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory Configuration")
	TSoftObjectPtr<UDataTable> ItemBehavioursTable;
	
	/// Inventory item definitions. This table must be created by right-click in the content area,
	/// choosing Adventure Tools > Item Type Definitions. Once the table is created set it here.
	/// Or click the drop-down and choose _Create new Asset_ > _Item Type Definitions_.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory Configuration")
	TSoftObjectPtr<UItemTypeDefs> ItemDefinitionsTable;
	
	DECLARE_DELEGATE_TwoParams(FCustomInventoryItemLoaded, FName /* ItemKind */, UInventoryItem * /* CustomInventoryItem */);
	
	FCustomInventoryItemLoaded CustomInventoryItemLoadedDelegate;

	/// Try to load a custom <code>UInventoryItem</code> for the item with the given name.
	void GetCustomInventoryItem(FName ItemKind);

private:
	FLoadSoftObjectPathAsyncDelegate LoadTableDelegate;
	FLoadSoftObjectPathAsyncDelegate LoadClassDelegate;
	
	UFUNCTION()
	void InventoryTableLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);

	UFUNCTION()
	void InventoryClassLoadCompleteHandler(const FSoftObjectPath& Path, UObject* Object);

	void GetCustomInventoryItemWithTable(FName ItemKind, UDataTable* DataTablePtr);
	
	void GetCustomInventoryItemWithClass(FName ItemKind, const UClass* InventoryItemClass);
	
	TArray<FName> TableOperationsQueue;
	TMap<FString, FName> ClassOperationsQueue;
	
public:
	/// Bind to this event to be notified of changes to the players inventory.
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FPlayerInventoryChanged PlayerInventoryChanged;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItemToInventory(FName ItemKind);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RemoveItemFromInventory(FName ItemKind);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void RemoveItemsFromInventory(const TSet<FName>& ItemsToRemove);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool IsInInventory(const FName &ItemToCheck) const;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	UItem* GetItemFromInventory(const FName &ItemToCheck);

	void GetInventoryItems(TArray<UItem*> &Items);

	int GetInventoryItemCount() const;
	
	/// Do not save a reference to this inventory object. It will get created and destroyed whenever
	/// the player saves or loads the game. To keep a reference safely, handle the 
	/// <code>PlayerInventoryChanged</code> event, and when the _ItemDisposition_ is 
	/// <code>Reloaded</code> get the new instance from here.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Inventory")
	UInventory *Inventory;
	
	/// All the tags currently set in the game
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Save Game")
	FGameplayTagContainer GameplayTags;
	
	// IGameplayTagAssetInterface interface.
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
private:
	FDelegateHandle OnInventoryChangedHandle;

	void CreateInventory();

	void DestroyInventory();

	void BindInventoryChangedHandlers();

	void InventoryChanged(FName ItemKind, EItemDisposition ItemDisposition);

	//////////////////////////////////
	///
	/// DOOR MANAGEMENT
	///

public:
	
	/// Call from a blueprint of a door to trigger loading the room.
	UFUNCTION(BlueprintCallable, Category="GameInstance")
	static void LoadRoom(ADoor *FromDoor);

	/// The door which the player most recently walked through into the current room.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GameInstance")
	ADoor *CurrentDoor;

	/// Set this value to the name of which ever level should be loaded at the game start
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameInstance")
	FName StartingLevelName = "TowerExterior"; 

	/// Set this value to the of which door the player should started at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GameInstance")
	FName StartingDoorLabel = "A1";

	/// TriggerRoomTransition to a room based on a door that has a new level name and door label
	void SetDestinationFromDoor(ADoor *NewCurrentDoor);

	/// TriggerRoomTransition to a room based on the level name and door label
	void SetLoadTarget(FName LevelName, FName DoorLabel);

	// The name of door the player character is at, or was most recently at.
	FName CurrentDoorLabel = FName("A1");

	// The name of the current lavel
	FName CurrentLevelName = FName("TowerExterior");

	//////////////////////////////////
	///
	/// TRANSITION CURRENT ROOM -> NEW ROOM VIA DOOR
	///

	/// Event to handle requests to load and transition from an old room
	/// to a new room based on the current door
	UFUNCTION(BlueprintCallable, Category="Room")
	void OnLoadRoom();

	/// Event for when a room is loaded
	UFUNCTION()
	void OnRoomLoaded();
	
	/// Event for when a room is unloaded
	UFUNCTION()
	void OnRoomUnloaded();

	/// Run the OnLoadRoom event to load a new level, and unload the current level.
	void TriggerRoomTransition();
	
	ERoomTransitionPhase GetRoomTransitionPhase() { return RoomTransitionPhase; }

	
	//////////////////////////////////
	///
	/// SAVE GAME
	///

	/// If true, when the game is launched a saved game will be checked for, and if it
	/// exists it will be loaded. Useful for debugging though.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GameInstance")
	bool ShouldCheckForSaveGameOnLoad = false;
	
	/// Store current game state into the <code>CurrentSaveGame</code> object ready for it to
	/// be serialised to persistent storage with the given name.
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void SaveGame();

	/// Retrieve current game state from the <code>CurrentSaveGame</code> object and make it
	/// active in the current game.
	UFUNCTION(BlueprintCallable, Category="SaveGame")
	void LoadGame();

	void RegisterHotSpotForSaveAndLoad(AHotSpot *HotSpot);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gameplay")
	TSubclassOf<UAdventureSave> SaveGameClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	UAdventureSave* CurrentSaveGame;
	
/**
	Connect a listener to this event to be notified when the game levels are loaded.

	<pre>	
	On game start:
	
	Member function called      | State at call
	--------------------------- | ------------------
	OnLoadRoom()                | GameNotStarted
	LoadStartingRoom()          | LoadStartingRoom
	OnRoomLoaded()              | NewRoomLoaded
	NewRoomDelay()              | DelayProcessing
	OnRoomLoadTimerTimeout()    | 
	SetupRoom()                 | 
	StartNewRoom()              |   RoomCurrent
			
	On transition from one room to the next room:

	Member function called      |    State 
	--------------------------- | ------------------
	OnLoadRoom()                | RoomCurrent
	LoadRoom()                  | LoadNewRoom
	OnRoomLoaded()              | NewRoomLoaded
	UnloadRoom()                | UnloadOldRoom
	OnRoomUnloaded()            | 
	NewRoomDelay()              | DelayProcessing
	OnRoomLoadTimerTimeout()    | 
	SetupRoom()                 |  
	StartNewRoom()              | RoomCurrent
	</pre>
	
	On Init() calling LoadGame() the StartingDoorLabel
	and StartingLevelName will be set to values from the save
	game. Then, when the AdventurePlayerController calls OnLoadRoom
	with GameNotStarted, the save game room will be loaded.
	Its important at this point that the Starting Level does _not_
	have Permanently Loaded set to true in the levels window.
*/
	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category="Gameplay")
	FRoomTransitioned RoomTransitionedDelegate;
	
private:
	
	TArray< TWeakObjectPtr< AHotSpot >> RegisteredHotSpots;

	UPROPERTY()
	TArray<FDataSaveRecord> AdventureSaves;
	
	//////////////////////////////////
	///
	/// LOAD ROOM
	///
	
	const FName OnLoadRoomName = "OnLoadRoom";
	const FName OnRoomLoadedName = "OnRoomLoaded";
	const FName OnRoomUnloadedName = "OnRoomUnloaded";
	
	ERoomTransitionPhase RoomTransitionPhase = ERoomTransitionPhase::GameNotStarted;
	
	/// Load up the room specified by the current door
	/// Currently this uses streaming levels, which means that the level is never
	/// truly unloaded. There's a persistent level underlying each room, which is
	/// there all the time. This means that the WorldSettings and in particular
	/// the game mode is fixed, in the whole game. To allow for larger games with
	/// more distinct different areas of rooms, we might need to add regular
	/// OpenLevel type loading, which is a long blocking call that completely
	/// unloads the previous level and loads the new one off disk. 
	void LoadRoom();

	/// Load up the room specified by the starting door
	void LoadStartingRoom();

	/// Unload the room specified by the current door
	void UnloadRoom();

	/// Run a short delay to allow scene to complete, and any geometry
	/// to be placed, spawned and stabilise.
	void NewRoomDelay();
	
	/// Setup the current room, find & load the door (move the player
	/// character to the relevant door). Hide the transition effect.
	/// Call this only after the delay to ensure the player character
	/// and all geometry is stable from any collisions.
	void SetupRoom();

	/// Restart play in the new room allowing input from the player.
	void StartNewRoom();

	void OnRoomLoadTimerTimeout();

	FLatentActionInfo GetLatentActionForHandler(FName EventName);
	
	FTimerHandle RoomLoadTimer;

	UPROPERTY(EditDefaultsOnly, Category="Room")
	float RoomLoadDelay = 0.5;
	
	//////////////////////////////////
	///
	/// UTILITIES
	///

public:
	UAdventureGameHUD *GetHUD();
	
private:
	ADoor *FindDoor(FName DoorLabel);

	void LogSaveGameStatus(USaveGame *SaveGame);
	
	UPROPERTY()
	UManagerProvider *ManagerProvider;
};
