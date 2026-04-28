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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock *InteractionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock *Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock *SaveIndicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage *Bullet;

	UFUNCTION(BlueprintCallable)
	void SetText(FText NewText);

	UFUNCTION(BlueprintCallable)
	void SetScore(FText NewText);

	UFUNCTION(BlueprintCallable)
	void HighlightText();

	UFUNCTION(BlueprintCallable)
	void UnhighlightText();

	UFUNCTION(Blueprintable)
	void ResetText();

	UFUNCTION(BlueprintCallable)
	void StartSaving();
	
	UFUNCTION(BlueprintCallable)
	void EndSaving(bool Success);

	UFUNCTION(BlueprintCallable)
	void StartLoading();
	
	UFUNCTION(BlueprintCallable)
	void EndLoading(bool Success);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Saving Indicator")
	float FlashTempo = 0.5f; // flash on then off every this many seconds.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Saving Indicator")
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FLinearColor TextHighlightColor = FLinearColor::Green;

	/** Color when normal - eg 7D652AFF */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	FLinearColor TextNormalColor = FLinearColor::Red;


private:
	void StartFlashing();

	void EndFlashing(bool Success);
	
	bool TextLocked;
};
