#pragma once

#include "CoreMinimal.h"
#include "Item.h"

#include "ItemTypeDef.generated.h"

/**
 * All the data for a unique item in the game. This is a row in the ItemTypeDefs container for the game.
 * When an instance of the item is instantiated in a game as a `UItem` then this data is used to create that instance.
 */
USTRUCT(BlueprintType)
struct FItemTypeDef
{
	GENERATED_USTRUCT_BODY()
	
	/// An internal unique name, which must be registered in the gameplay tags
	/// via Edit > ProjectSettings > Manage Gameplay Tags, and added to the 
	/// <code>Item.Kind</code> category, eg: "Item.Kind.FancyBox"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTypeDef, meta=(Categories="Item.Kind"))
	FGameplayTag UniqueName;
	
	/// A short user facing description to help game designers and also serve as a default text in error messages
	/// for example "Pickled gherkin"
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTypeDef)
	FText TypeDescription;
		
	/// Must be set to a valid Item 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTypeDef)
	TSoftObjectPtr<UItem> Item;
	
	/// Set this to false to disable this item, and prevent it from being used
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTypeDef)
	bool bEnabled = true;

	/// Set to true when the Item Type is initialized with valid data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemTypeDef)
	bool bValid = false;
};
