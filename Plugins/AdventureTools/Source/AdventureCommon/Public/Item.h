// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "DescribableItem.h"
#include "DoorState.h"
#include "GameplayTagContainer.h"
#include "ItemDataList.h"
#include "Internationalization/StringTableRegistry.h"
#include "UObject/Object.h"
#include "Item.generated.h"

#define LOCTEXT_NAMESPACE "FAdventureCommonModule"

class UPaperSprite;

/**
 * An item in a game. This is instantiated based on an `FItemTypeDef`, which is uniquely identified
 * by the FName property `ItemTypeDef`.
 */
UCLASS()
class ADVENTURECOMMON_API UItem : public UObject, public IDescribableItem
{
	GENERATED_BODY()
public:
	//////////////////////////////////
	///
	/// DESCRIBABLE ITEM
	///

	virtual FText GetShortDescription() const override { return ShortDescription;}
	
	virtual FText GetLongDescription() const override { return Description; }
	
	virtual FName GetItemKind() const override { return ItemTypeDef.GetTagLeafName(); }
	
	/// What item is this? Currently its only supported to have a single one 
	/// of each item in the game. If there is a "Knife", and some other thing 
	/// that is a knife is needed in another part of the game it must be "Switchblade" 
	/// or some other name.
	/// 
	/// @invariant Must be one of the names listed in ItemTypeDefs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Configuration", meta=(Categories="Item.Kind"))
	FGameplayTag ItemTypeDef;
	
	//////////////////////////////////
	///
	/// ITEM HANDLING
	///

	/// Longer description used in "Look at" to make the item more real.
	/// eg "Blunt red knife with blood on it" - "Old tattered book covered with runes".
	/// By default uses the `DefaultItemDescriptionText` value in the `ItemStrings` string table.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	FText Description = LOCTEXT("DefaultItemDescriptionText", "It doesn't look like anything");

	/// Very short description to distinguish this thing from others of the same kind
	/// eg "Blunt red knife" - "Old tattered book". By default uses the
	/// `DefaultItemDescriptionText` value in the `ItemStrings` string table.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	FText ShortDescription = LOCTABLE("ItemStrings", "DefaultItemDescriptionText");

	/// Various tags for the state and past actions done on this hotspot.
	/// For example can set "History.Triggered.LookAt" to enforce that a score increment
	/// is only given once, the first time the item is looked at.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scripting", meta = (Categories = "History"))
	FGameplayTagContainer HistoryTags;
	
	/// Thumbnail image to represent this item inside the inventory.
	/// Images used for the item while in the level or scene should
	/// be attached to a hotspot pickup instead.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	TObjectPtr<UPaperSprite> Thumbnail;
	
	/// An item kind that can meaningfully interact with this one. Used when
	/// items are combined in the inventory, such as _Use knife on pickle_.
	/// This has no effect on other verbs.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling", meta=(Categories="Item.Kind"))
	FGameplayTag InteractableItemName;
	
	//////////////////////////////////
	///
	/// ITEM DISPOSITION
	///

	/// Is the item open or close, or locked?  For items where those concepts make
	/// no sense this should stay in the <code>Unknown</code> state.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	EDoorState DoorState = EDoorState::Unknown;

	/// If this Inventory Item can interact with <code>ItemToInteract</code> then
	/// return true, otherwise return false. 
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	bool CanInteractWith(const UItem* ItemToInteract) const
	{
		return ItemToInteract->InteractableItemName == ItemTypeDef || ItemToInteract->ItemTypeDef == InteractableItemName;
	}
    
	/// Data Assets for determining results of activating this item. This should be
	/// a list of <b>instances</b> of the <code>ItemDataAsset</code> sub-class, not the
	/// class itself. Right-click in the content drawer, and choose <i>Miscellaneous - Data Asset</i>
	/// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	FItemDataList Activations;
};

#undef LOCTEXT_NAMESPACE