#include "Items/ItemTableProvider.h"

#include "AdventureTools.h"
#include "Item.h"

UItemTableProvider::UItemTableProvider(FObjectInitializer const&Initializer)
	: UObject(Initializer)
{
#if WITH_EDITOR
	if (ItemBehavioursTable.IsNull())
	{
		UE_LOG(LogAdventureGame, Warning, 
			TEXT("ItemBehavioursTable has not been set! Blueprint this class to define item behaviours"))
	}
	if (ItemDefinitionsTable.IsNull())
	{
		UE_LOG(LogAdventureGame, Warning, TEXT("ItemDefinitionsTable has not been set! Blueprint this class to define items."))
	}
#endif
}

TSoftObjectPtr<UDataTable> UItemTableProvider::GetItemBehavioursTable()
{
	return ItemBehavioursTable;
}

TSoftObjectPtr<UItemTypeDefs> UItemTableProvider::GetItemDefinitionsTable()
{
	return ItemDefinitionsTable;
}

void UItemTableProvider::RegisterWithGameInstance(UItem* InventoryItem)
{
	if (auto AGI = GameInstance.Get())
	{
		AGI->RegisterReferencedObject(InventoryItem);
	}
}

void UItemTableProvider::UnregisterFromGameInstance(UItem* InventoryItem)
{
	if (auto AGI = GameInstance.Get())
	{
		return AGI->UnregisterReferencedObject(InventoryItem);
	}
}
