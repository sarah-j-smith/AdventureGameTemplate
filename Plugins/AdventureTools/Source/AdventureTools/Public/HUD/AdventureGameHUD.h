// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Items/InventoryManager.h"
#include "AdventureGameHUD.generated.h"

struct IManagerProvider;
class USphereComponent;
class UImage;
class UBarkText;
class UPromptList;
class UInventoryUI;
class UVerbsUI;
class UInteractionHUD;
class UInteractionNotifier;
class AAdventureGameModeBase;
class UAdventureGameInstance;
class ACommandManager;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UAdventureGameHUD : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual void NativeOnInitialized() override;
public:
	static UAdventureGameHUD *Create(APlayerController *PlayerController, TSubclassOf<UAdventureGameHUD> AdventureHUDClass);

	/// Subscribe to messages for command & action updates to show in the interaction display.
	virtual void BindCommandHandlers(ACommandManager *CommandManager);

	/// Subscribe to messages for changes in the player score to show in the score UI
	virtual void BindScoreHandlers(AAdventureGameModeBase* AdventureGameMode);

	/// Subscribe to messages for commands from the player, used to open or close
	/// the conversation UI, or signal that a UI interaction happened to dismiss the
	/// current bark or NPC conversation
	virtual void BindNotifierHandlers(UInteractionNotifier* Notifier);

	/// Bindings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UInteractionHUD *InteractionUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UVerbsUI *VerbsUI;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UInventoryUI *InventoryUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UWidgetSwitcher *UISwitcher;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UPromptList *PromptList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UBarkText *Bark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Adventure HUD")
	UImage *BlackScreen;

	UFUNCTION()
	void HandleInventoryChanged(FName ItemKind, EItemDisposition Disposition);

	UFUNCTION()
	void HandleScoreChanged(int32 Score);

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void UpdateInteractionTextEvent();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void UpdateSaveGameIndicatorEvent(ESaveGameStatus SaveGameStatus, bool Success);

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void UpdateInventoryTextEvent();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void BeginActionEvent();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void InterruptActionEvent();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void OnUserInteracted();
	
	void ShowBlackScreen();

	void HideBlackScreen();

	void SetInteractionText();

	void SetInventoryText();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void ShowPromptList();

	UFUNCTION(BlueprintCallable, Category = "Adventure HUD")
	void HidePromptList();

	virtual void AddBarkText(const FText &BarkText, USphereComponent *Position,
		TOptional<FColor> TextColor = TOptional<FColor>());

	virtual void AddBarkText(const TArray<FText> &BarkTextArray, USphereComponent* Position, TOptional<FColor> TextColor);

	void ClearBarkText();
	
private:
    bool IsMobileTouch = false;

	UPROPERTY()
	UWidget *DefaultWidget = nullptr;
	
	TSharedPtr<IManagerProvider> ManagerProvider;
	
	TSharedPtr<IInventoryManager> InventoryManager;
};
