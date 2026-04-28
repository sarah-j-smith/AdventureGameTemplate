// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DataSaveRecord.h"
#include "GameplayTagContainer.h"
#include "ItemKind.h"

#include "GameFramework/SaveGame.h"

#include "AdventureSave.generated.h"

class UAdventureGameInstance;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UAdventureSave : public USaveGame
{
    GENERATED_BODY()
public:
    /// All the tags currently set in the game
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Save Game")
    FGameplayTagContainer AdventureTags;
    
    /// When this is loaded where should the player start? This is probably the
    /// most recent door they came through.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Starting Level")
    FName StartingDoorLabel;

    /// When this is loaded what level should the player be on? Should be the level
    /// the player was on when the game was saved.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Starting Level")
    FName StartingLevel;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Items")
    TArray<EItemKind> Inventory;

    /// Called after known core game values are saved into this object by
    /// a call to <code>SaveGame</code> on the Game Instance. Respond to this
    /// event by **storing into variables on this instance** all the state that
    /// needs to be preserved.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Game")
    void OnAdventureSave(const UAdventureGameInstance *GameInstance);

    /// Called after known core game values are loaded from this object by
    /// a call to <code>LoadGame</code> on the Game Instance. Respond to this
    /// event by **loading from variables on this instance** all the state that
    /// needs to be restored during a game load.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Save Game")
    void OnAdventureLoad(const UAdventureGameInstance *GameInstance);

    UPROPERTY()
    TArray<FDataSaveRecord> AdventureSaves;
};
