// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DoorState.h"

#include "Kismet/BlueprintFunctionLibrary.h"

#include "AdvBlueprintFunctionLibrary.generated.h"

class UItemManager;
class ACommandManager;
class AAdventureCharacter;
class AHotSpot;
class UAdventureGameInstance;
enum class EItemKind: uint8;
class UInventoryItem;
class AAdventurePlayerController;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UAdvBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Gameplay", meta = (WorldContext = "WorldContextObject"))
    static void AddToScore(const UObject* WorldContextObject, int32 ScoreIncrement);

    /// Get the controller used for handling click and touch input, and for playing animations on the character
    /// This function is a bit slow, so don't use it in a loop.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Controllers",
        meta = (WorldContext = "WorldContextObject"))
    static AAdventurePlayerController* GetAdventureController(const UObject* WorldContextObject);

    /// Get the manager used for handling player commands; verbs and item actions.
    /// This function is a bit slow, so don't use it in a loop.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Controllers", meta = (WorldContext = "WorldContextObject"))
    static ACommandManager* GetCommandManager(const UObject* WorldContextObject);
    
    /// Get the manager used for handling items in the inventory
    /// This function is a bit slow, so don't use it in a loop.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Controllers", meta = (WorldContext = "WorldContextObject"))
    static UItemManager* GetItemManager(const UObject* WorldContextObject);

    /// Get the player to "bark" a message, which prints on screen above their head
    UFUNCTION(BlueprintCallable, Category = "Player Actions", meta = (WorldContext = "WorldContextObject"))
    static void PlayerBark(const UObject* WorldContextObject, FText BarkText);

    UFUNCTION(BlueprintCallable, Category = "Player Actions", meta = (WorldContext = "WorldContextObject"))
    static void ClearVerb(const UObject* WorldContextObject);

    /// Add a new item to the inventory and return a reference to it if successful.
    /// @param ItemToAdd The kind of item to add. The text descriptions for each kind can be added in the text resource.
    UFUNCTION(BlueprintCallable, Category = "Player Actions", meta = (WorldContext = "WorldContextObject"))
    static UInventoryItem* AddToInventory(const UObject* WorldContextObject, EItemKind ItemToAdd);

    UFUNCTION(BlueprintCallable, Category = "Player Actions", meta = (WorldContext = "WorldContextObject"))
    static void RemoveFromInventory(const UObject* WorldContextObject, EItemKind ItemToRemove);

    UFUNCTION(BlueprintPure, Category = "Debug", meta = (WorldContext = "WorldContextObject"))
    static int32 PIEInstance(const UObject* WorldContextObject);

    UFUNCTION(BlueprintPure, Category = "Debug")
    static FString GetProjectVersion();

    UFUNCTION(BLueprintCallable, Category = "Player Actions", BlueprintPure)
    static float GetBarkTime(const FString& BarkText);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Controllers", meta = (WorldContext = "WorldContextObject"))
    static UAdventureGameInstance* GetAdventureInstance(const UObject* WorldContextObject);

    /**
     * Check based on state if the door or item can be locked
     * @param DoorState Current state of the door or item
     * @return true if the door or item can be locked
     */
    UFUNCTION(BLueprintCallable, Category = "Door State", BlueprintPure)
    static bool CanLock(EDoorState DoorState) { return CanLockDoorOrItem(DoorState); }

    /**
     * Check if the door or item can be unlocked.
     * @param DoorState Current state of the door or openable item
     * @return true if the door or item can be unlocked
     */
    UFUNCTION(BLueprintCallable, Category = "Door State", BlueprintPure)
    static bool CanUnlock(EDoorState DoorState) { return CanUnlockDoorOrItem(DoorState); };

    /**
     * Check based on state if the door or item can be locked
     * @param DoorState Current state of the door or item
     * @return true if the door or item can be locked
     */
    UFUNCTION(BLueprintCallable, Category = "Door State", BlueprintPure)
    static bool CanOpen(EDoorState DoorState) { return CanOpenDoorOrItem(DoorState); };

    /**
     * Check based on state if the door or item can be locked
     * @param DoorState Current state of the door or item
     * @return true if the door or item can be locked
     */
    UFUNCTION(BLueprintCallable, Category = "Door State", BlueprintPure)
    static bool CanClose(EDoorState DoorState) { return CanCloseDoorOrItem(DoorState); };

    /**
     * Check based on distance of the <code>AdventureCharacter</code> from the
     * <code>HotSpot</code> if they are <i>close</i> or not, within the given tolerance.
     * Note that the Z value of HotSpot and Character position are ignored.
     * @param HotSpot Actor in the level to check against.
     * @param AdventureCharacter Character to check the distance of.
     * @param Tolerance Fudge factor to apply, if the distance is less than this, the character is close. Defaults to 5.0f.
     * @return true if the Character is close to the HotSpot
     */
    UFUNCTION(BLueprintCallable, Category = "Testing", BlueprintPure)
    static bool IsCharacterCloseToHotSpot(AHotSpot* HotSpot, AAdventureCharacter* AdventureCharacter,
        float Tolerance = 5.0f);
    
    /**
     * Check based on distance of the <code>AdventureCharacter</code> from the
     * <code>Location</code> if they are close or not, within the given tolerance.
     * Note that the Z value of Location and Character is ignored.
     * @param Location Vector position in world space in the level to check against.
     * @param AdventureCharacter Character to check the distance of.
     * @param Tolerance Fudge factor to apply, if the distance is less than this, the character is close. Defaults to 5.0f.
     * @return true if the Character is close to the HotSpot
     */
    UFUNCTION(BLueprintCallable, Category = "Testing", BlueprintPure)
    static bool IsCharacterCloseToLocation(FVector Location, AAdventureCharacter* AdventureCharacter,
        float Tolerance = 5.0f);
    
    /**
     * Get the distance of the <code>AdventureCharacter</code> from the
     * <code>Location</code> ignoring the Z value. If the <code>AdventureCharacter</code>
     * is null, or its capsule is at 0, 0, 0 it is an error.
     * @param Location Vector position in world space in the level to check against.
     * @param AdventureCharacter Character to check the distance of.
     * @return float value of the distance, or infinity if character is nullptr or at 
     */
    UFUNCTION(BLueprintCallable, Category = "Testing", BlueprintPure)
    static float GetCharacterDistanceToLocation(FVector Location, AAdventureCharacter* AdventureCharacter);
};
