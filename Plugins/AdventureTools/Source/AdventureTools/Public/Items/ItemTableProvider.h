#pragma once

#include "IItemTableProvider.h"

#include <CoreMinimal.h>

#include <ItemTableProvider.generated.h>

UCLASS(BlueprintType, Blueprintable)
class UItemTableProvider: public UObject, public IItemTableProvider
{
	GENERATED_UCLASS_BODY()
	
	TWeakObjectPtr<UGameInstance> GameInstance;

	virtual ~UItemTableProvider() override = default;
	
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
	
	/**
	* Custom inventory item behaviours. This table is optional but must be set to a new Data Table,
	* with FItemRow as the row type, if it is used. The entries in the table map _names_ of 
	* items in the game to sub-classes (usually Blueprints) of <code>UInventoryItem</code> which 
	* can have a Blueprint script attached to provide custom behaviours. 
	 */
	virtual TSoftObjectPtr<UDataTable> GetItemBehavioursTable() override;

	/**
	* Inventory item definitions. This table must be created by right-click in the content area,
	* choosing Adventure Tools > Item Type Definitions. Once the table is created set it here.
	* Or click the drop-down and choose _Create new Asset_ > _Item Type Definitions_.
	 */
	virtual TSoftObjectPtr<UItemTypeDefs> GetItemDefinitionsTable() override;

	/**
	 * Register an item with the Game Instance to avoid it being garbage collected
	 * @param InventoryItem Item to be registered
	 */
	virtual void RegisterWithGameInstance(UItem* InventoryItem) override;

	/**
	 * De-register an item previously registered.
	 * @param InventoryItem Item to be deregistered
	 */
	virtual void UnregisterFromGameInstance(UItem* InventoryItem) override;
};
