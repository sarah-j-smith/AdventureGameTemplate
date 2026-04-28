// (c) 2025 Sarah Smith


#include "HUD/ItemSlot.h"

#include "Components/Image.h"
#include "Player/AdventurePlayerController.h"
#include "Items/InventoryItem.h"
#include "Player/ItemManager.h"


void UItemSlot::NativeOnInitialized()
{
	ItemButton->OnClicked.AddDynamic(this, & UItemSlot::HandleOnClicked);
	ItemButton->OnHovered.AddDynamic(this, & UItemSlot::HandleOnHover);
	ItemButton->OnUnhovered.AddDynamic(this, & UItemSlot::HandleOnUnhover);

	if (!HasItem)
	{
		ItemSlot->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlot::AddItem(UInventoryItem* AnInventoryItem)
{
	if (AnInventoryItem != nullptr)
	{
		HasItem = true;
		this->InventoryItem = AnInventoryItem;
		SetButtonImageFromInventoryItem(AnInventoryItem);
		ItemSlot->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemSlot::RemoveItem()
{
	if (HasItem)
	{
		HasItem = false;
		this->InventoryItem = nullptr;
		ItemSlot->SetBrush(SavedStyle);
		ItemSlot->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlot::HandleOnClicked()
{
	if (ACommandManager *Command = GetCommandManager())
	{
		if (HasItem)
		{
			Command->HandleInventoryItemClicked(this);
		}
		else
		{
			// Clicked on an empty slot so cancel whatever it was doing.
			Command->InterruptCurrentAction();
		}
	}
}

void UItemSlot::HandleOnHover()
{
	if (HasItem)
	{
		if (UItemManager *ItemManager = GetItemManager())
		{
			ItemManager->MouseEnterInventoryItem(this);
		}		
	}
}

void UItemSlot::HandleOnUnhover()
{
	if (UItemManager *ItemManager = GetItemManager())
	{
		ItemManager->MouseLeaveInventoryItem();
	}		
}

void UItemSlot::SetButtonImageFromInventoryItem(const UInventoryItem* AnInventoryItem)
{
	SavedStyle = ItemSlot->GetBrush();
	FSlateBrush NewBrush = SavedStyle;
	NewBrush.DrawAs = ESlateBrushDrawType::Type::Image;
#if WITH_EDITOR
	NewBrush.SetResourceObject(AnInventoryItem->Thumbnail->GetSourceTexture());
#else
	NewBrush.SetResourceObject(AnInventoryItem->Thumbnail->GetBakedTexture());
#endif
	ItemSlot->SetBrush(NewBrush);
}

