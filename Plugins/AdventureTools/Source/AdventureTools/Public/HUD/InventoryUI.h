// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "ItemSlot.h"
#include "Blueprint/UserWidget.h"

#include "InventoryUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryUIChanged);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(BlueprintAssignable)
	FInventoryUIChanged InventoryUIChanged;
	
	//////////////////////////////////
	///
	/// ARROWS
	///
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Arrows")
	UButton *UpArrowButton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Arrows")
	UButton *DownArrowButton;

	UFUNCTION(BlueprintCallable, Category = "Arrows")
	void OnDownArrowButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Arrows")
	void OnUpArrowButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Inventory Slots")
	void PopulateInventory(bool ScrollToLastAdded = false);

	UFUNCTION(BlueprintCallable, Category = "Inventory Slots")
	UItemSlot *GetFromInventory(EItemKind ItemKind) const;

	/// Row of inventory displayed in the top row of slots.
	/// Will be zero unless there are more than 8 items in the inventory.
	/// When this is non-zero the arrow buttons can be used to see other rows.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arrows")
	int CurrentRowIndex = 0;
	
	/// The max value of CurrentRowIndex - ceil((inventory_count - 8)/4)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arrows")
	int MaxRowIndex = 0;
	
	/// The current number of items in the inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Arrows")
	int InventoryCount = 0;
	
	//////////////////////////////////
	///
	/// INVENTORY SLOTS
	///

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory Slots")
	TArray<UItemSlot *> InventorySlots;

	void AddSlotsToArray();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *TopItem1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *TopItem2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *TopItem3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *TopItem4;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *BottomItem1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *BottomItem2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *BottomItem3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Inventory Slots")
	UItemSlot *BottomItem4;
};
