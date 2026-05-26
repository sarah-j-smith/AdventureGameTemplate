// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Engine/TimerHandle.h"

#include "InteractionHUD.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UInteractionHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	UTextBlock *InteractionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	UTextBlock *Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	UTextBlock *SaveIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	UImage *Bullet;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetText(FText NewText);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetScore(FText NewText);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void HighlightText();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void UnhighlightText();

	UFUNCTION(Blueprintable, Category = "Interaction")
	void ResetText();

	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void StartSaving();
	
	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void EndSaving(bool Success);

	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void StartLoading();
	
	UFUNCTION(BlueprintCallable, Category = "Save and Load")
	void EndLoading(bool Success);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save and Load")
	float FlashTempo = 0.5f; // flash on then off every this many seconds.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Save and Load")
	float ReturnToRestTime = 1.5f;

	bool IsSaving = false;

	bool IsSaveIndicatorFlashOn = false;

	FTimerHandle FlashTimerHandle;

	UFUNCTION()
	void FlashTimerTimeout();
	
	FTimerHandle ReturnToRestTimerHandle;
	
	UFUNCTION()
	void ReturnToRestTimerTimeout();
	
	/** Color when highlighted - eg D79115FF */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	FLinearColor TextHighlightColor = FLinearColor::Green;

	/** Color when normal - eg 7D652AFF */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Interaction")
	FLinearColor TextNormalColor = FLinearColor::Red;

private:
	void StartFlashing();

	void EndFlashing(bool Success);
	
	bool TextLocked;
};
