// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/TimerHandle.h"

#include "DialogPrompt.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UDialogPrompt : public UUserWidget
{
    GENERATED_BODY()
public:
    virtual void NativeOnInitialized() override;
    
    /// The bullet at the start of each line
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UImage *PromptBullet;

    /// The button to activate the prompt
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UButton *PromptButton;

    /// The text of the button
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
    UTextBlock *PromptText;

    UFUNCTION(BlueprintCallable)
    void SetText(const FText &TextToSet);

    UFUNCTION(BlueprintCallable)
    void HighlightText();

    UFUNCTION(BlueprintCallable)
    void UnhighlightText();

    UFUNCTION(BlueprintCallable)
    void HidePrompt();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool HasBeenUsed;
    
    UFUNCTION()
    void HandleOnHover();
    
    UFUNCTION()
    void HandleOnUnhover();

    void FlashPrompt();
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float FlashTime = 0.6f;
    
    UFUNCTION(BlueprintCallable)
    void PromptTimerTimeout();
    
    FTimerHandle PromptTimerHandle;
};
