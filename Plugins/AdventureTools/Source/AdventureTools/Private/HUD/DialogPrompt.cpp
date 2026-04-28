// (c) 2025 Sarah Smith


#include "HUD/DialogPrompt.h"

#include "Constants.h"
#include "Components/Image.h"

void UDialogPrompt::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UDialogPrompt::SetText(const FText &TextToSet)
{
    SetVisibility(ESlateVisibility::Visible);
    PromptText->SetText(TextToSet);
}

void UDialogPrompt::HighlightText()
{
    PromptText->SetColorAndOpacity(G_UI_Highlight_Colour);
    PromptBullet->SetBrushTintColor(G_UI_Highlight_Colour);
}

void UDialogPrompt::UnhighlightText()
{
    PromptText->SetColorAndOpacity(HasBeenUsed ? G_UI_Disabled_Colour : G_UI_Dimmed_Colour);
    PromptBullet->SetBrushTintColor(HasBeenUsed ? G_UI_Disabled_Colour : G_UI_Dimmed_Colour);
}

void UDialogPrompt::HidePrompt()
{
    PromptText->SetText(FText::GetEmpty());
    SetVisibility(ESlateVisibility::Hidden);
}

void UDialogPrompt::HandleOnHover()
{
    HighlightText();
}

void UDialogPrompt::HandleOnUnhover()
{
    UnhighlightText();
}

void UDialogPrompt::FlashPrompt()
{
    GetWorld()->GetTimerManager().SetTimer(PromptTimerHandle, this, &UDialogPrompt::PromptTimerTimeout,
        FlashTime, false);
    HighlightText();
    SetIsEnabled(true);
}

void UDialogPrompt::PromptTimerTimeout()
{
    UnhighlightText();
    SetIsEnabled(false);
}

