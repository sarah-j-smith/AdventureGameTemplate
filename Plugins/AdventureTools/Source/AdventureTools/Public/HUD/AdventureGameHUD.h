// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Items/ItemManagerProvider.h"

#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "AdventureGameHUD.generated.h"

enum class EItemKind : uint8;
class UImage;
class UAdventureGameInstance;
class USphereComponent;
class AAdventureGameModeBase;
class UInteractionNotifier;
class UVerbsUI;
class UInteractionHUD;
class UPromptList;
class UInventoryUI;
class UBarkText;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UAdventureGameHUD : public UUserWidget, public IItemManagerProvider
{
	GENERATED_BODY()
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeOnInitialized() override;
public:
	static UAdventureGameHUD *Create(APlayerController *PlayerController, TSubclassOf<UAdventureGameHUD> AdventureHUDClass);

	/// Subscribe to messages for command & action updates to show in the interaction display.
	void BindCommandHandlers(ACommandManager *CommandManager);

	/// Subscribe to messages for changes in the player inventory to show in the inventory UI
	void BindInventoryHandlers(UAdventureGameInstance* AdventureGameInstance);

	/// Subscribe to messages for changes in the player score to show in the score UI
	void BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode);

	/// Subscribe to messages for commands from the player, used to open or close
	/// the conversation UI, or signal that a UI interaction happened to dismiss the
	/// current bark or NPC conversation
	void BindNotifierHandlers(UInteractionNotifier* Notifier);

	/// Bindings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UInteractionHUD *InteractionUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UVerbsUI *VerbsUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UInventoryUI *InventoryUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UWidgetSwitcher *UISwitcher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UPromptList *PromptList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UBarkText *Bark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage *BlackScreen;

	UFUNCTION()
	void HandleInventoryChanged(EItemKind ItemKind, EItemDisposition Disposition);

	UFUNCTION()
	void HandleScoreChanged(int32 Score);

	UFUNCTION(BlueprintCallable)
	void UpdateInteractionTextEvent();

	UFUNCTION(BlueprintCallable)
	void UpdateSaveGameIndicatorEvent(ESaveGameStatus SaveGameStatus, bool Success);

	UFUNCTION(BlueprintCallable)
	void UpdateInventoryTextEvent();

	UFUNCTION(BlueprintCallable)
	void BeginActionEvent();

	UFUNCTION(BlueprintCallable)
	void InterruptActionEvent();

	UFUNCTION(BlueprintCallable)
	void OnUserInteracted();
	
	void ShowBlackScreen();

	void HideBlackScreen();

	void SetInteractionText();

	void SetInventoryText();

	UFUNCTION(BlueprintCallable)
	void ShowPromptList();

	UFUNCTION(BlueprintCallable)
	void HidePromptList();

	void AddBarkText(const FText &BarkText, USphereComponent *Position,
		TOptional<FColor> TextColor = TOptional<FColor>());

	void AddBarkText(const TArray<FText> &BarkTextArray, USphereComponent* Position, TOptional<FColor> TextColor);

	void ClearBarkText();
	
private:
    bool IsMobileTouch = false;

	UPROPERTY()
	UWidget *DefaultWidget = nullptr;
};
