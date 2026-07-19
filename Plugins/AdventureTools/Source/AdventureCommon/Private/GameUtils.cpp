#include "GameUtils.h"

#include "Constants.h"
#include "DescribableItem.h"
#include "Internationalization/StringTableRegistry.h"

#include "Misc/Guid.h"

bool FGameUtils::VectorsDiffer(const FVector2D& Current, const FVector2D& Previous)
{
    return VectorsDifferBy(std::numeric_limits<double>::epsilon(), Current, Previous);
}

bool FGameUtils::VectorsDifferBy(const double Tolerance, const FVector2D& Current, const FVector2D& Previous)
{
    return fabs(Previous.X - Current.X) >= Tolerance || fabs(Previous.Y - Current.Y) >= Tolerance;

}

bool FGameUtils::PlayerIsAt(const APawn* Player, const FVector& Location, const double Tolerance)
{
    const FVector PlayerLocation = Player->GetActorLocation();
    return VectorsDifferBy(Tolerance, FVector2D(PlayerLocation.X, PlayerLocation.Y), FVector2D(Location.X, Location.Y));
}

uint32 FGameUtils::GetDisplayTimeForString(const FString &BarkText)
{
    int LineCount = 1;
    if (BarkText.Contains(NEW_LINE_SEPARATOR))
    {
        for (auto Element : BarkText)
        {
            if (Element == '\n')
            {
                ++LineCount;
            }
        }
    }
    float LineDelay = LineCount * BARK_LINE_DELAY;
    const int LetterCount = BarkText.Len();
    if (LetterCount <= SHORT_LETTER_COUNT) return LineDelay + SHORT_BARK_TIME;
    if (LetterCount <= MEDIUM_LETTER_COUNT) return LineDelay + MEDIUM_BARK_TIME;
    if (LetterCount <= LONG_LETTER_COUNT) return LineDelay + LONG_BARK_TIME;
    if (LetterCount <= EXTRA_LONG_LETTER_COUNT) return LineDelay + EXTRA_LONG_BARK_TIME;
    return ceilf(XXLONG_BARK_MULT * static_cast<float>(LetterCount) + LineDelay);
}

uint32 FGameUtils::GetUUID()
{
    const FGuid NewGuid = FGuid::NewGuid();
    return NewGuid.A;
}

FText FGameUtils::GetScoreText(int32 Score)
{
    FFormatNamedArguments ScoreArgs;
    ScoreArgs.Add("ScoreValue", Score);
    return FText::Format(LOCTABLE(UI_STRINGS_KEY, G_SCORE_TEMPLATE_KEY), ScoreArgs);
}

FText FGameUtils::GetGivingItemText(const IDescribableItem* CurrentItem, const IDescribableItem* TargetItem,
                                      const IDescribableItem* HotSpot)
{
    verifyf(CurrentItem, TEXT("GetGivingItemString expects CurrentItem to be non-null"));
    FText SubjectText = CurrentItem->GetShortDescription();
    FFormatNamedArguments VerbArgs;
    VerbArgs.Add("Subject", SubjectText);
    if (HotSpot == nullptr && TargetItem == nullptr)
    {
        return FText::Format(LOCTABLE(ITEM_STRINGS_KEY, G_GIVE_TO_PREVIEW), VerbArgs);
    }
    if (TargetItem != nullptr && TargetItem->GetItemKind() == CurrentItem->GetItemKind())
    {
        return FText::Format(LOCTABLE(ITEM_STRINGS_KEY, G_GIVE_TO_SELF_DEFAULT_KEY), VerbArgs);
    }
    FText TargetText = HotSpot ? HotSpot->GetShortDescription() : TargetItem->GetShortDescription();
    VerbArgs.Add("Object", TargetText);
    return FText::Format(LOCTABLE(ITEM_DESCRIPTIONS_KEY, G_GIVE_SUBJECT_TO_OBJECT_KEY), VerbArgs);
}

FText FGameUtils::GetUsingItemText(const IDescribableItem* CurrentItem, const IDescribableItem* TargetItem,
                                     const IDescribableItem* HotSpot)
{
    verifyf(CurrentItem, TEXT("GetGivingItemString expects CurrentItem to be non-null"));
    FText SubjectText = CurrentItem->GetShortDescription();
    FFormatNamedArguments VerbArgs;
    VerbArgs.Add("Subject", SubjectText);
    if (HotSpot == nullptr && TargetItem == nullptr)
    {
        return FText::Format(LOCTABLE(ITEM_STRINGS_KEY, G_USE_ON_PREVIEW), VerbArgs);
    }
    if (TargetItem != nullptr && TargetItem->GetItemKind() == CurrentItem->GetItemKind())
    {
        return FText::Format(LOCTABLE(ITEM_STRINGS_KEY, G_USE_ON_SELF_DEFAULT_KEY), VerbArgs);
    }
    FText TargetText = HotSpot ? HotSpot->GetShortDescription() : TargetItem->GetShortDescription();
    VerbArgs.Add("Object", TargetText);
    return FText::Format(LOCTABLE(ITEM_DESCRIPTIONS_KEY, G_USE_SUBJECT_ON_OBJECT_KEY), VerbArgs);
}

FText FGameUtils::GetVerbWithItemText(const IDescribableItem* CurrentItem, const EVerbType Verb)
{
    verifyf(CurrentItem, TEXT("GetVerbWithItemString expects CurrentItem to be non-null"));
    FText ItemText = CurrentItem->GetShortDescription();
    if (ItemText.IsEmpty()) ItemText = FText::FromString(CurrentItem->GetItemKind().ToString());
    FFormatNamedArguments VerbArgs;
    VerbArgs.Add("Verb", VerbGetDescriptiveString(Verb));
    VerbArgs.Add("Subject", ItemText);
    return FText::Format(LOCTABLE(ITEM_DESCRIPTIONS_KEY, G_VERB_SUBJECT_KEY), VerbArgs);
}

FText FGameUtils::GetVerbWithHotSpotText(const IDescribableItem* HotSpot, const EVerbType Verb)
{
    verifyf(HotSpot, TEXT("GetVerbWithHotSpotString expects HotSpot to be non-null"));
    FFormatNamedArguments VerbArgs;
    VerbArgs.Add("Verb", VerbGetDescriptiveString(Verb));
    VerbArgs.Add("Subject", HotSpot->GetShortDescription());
    return FText::Format(LOCTABLE(ITEM_DESCRIPTIONS_KEY, G_VERB_SUBJECT_KEY), VerbArgs);
}

TArray<FText> FGameUtils::NewLineSeparatedToArrayText(const FText& NewText)
{
    FString Line = NewText.ToString();
    if (!Line.Contains(NEW_LINE_SEPARATOR)) return TArray<FText>({ NewText });
    TArray<FString> NewLines = NewLineSeparatedToArrayString(Line);
    TArray<FText> OutText;
    Algo::Transform(NewLines, OutText, [](const FString& Str){ return FText::FromString(Str); });
    return OutText;
}

TArray<FString> FGameUtils::NewLineSeparatedToArrayString(const FString& NewString)
{
    if (NewString.Contains(NEW_LINE_SEPARATOR))
    {
        TArray<FString> NewLines;
        FString First, Rest, Buf = NewString;
        while (Buf.Split(NEW_LINE_SEPARATOR, &First, &Rest))
        {
            NewLines.Add(First.TrimStartAndEnd());
            Buf = Rest;
        }
        NewLines.Add(Buf.TrimStartAndEnd());
        return NewLines;
    }
    return TArray({ NewString });
}

TArray<FText> FGameUtils::WrapTextLinesToMaxCharacters(const FText& NewText, const int32 MaxLength)
{
    TArray<FText> WrappedLines;
    FString CurrentLine = NewText.ToString();
    while (CurrentLine.Len() > MaxLength)
    {
        int32 SplitPoint = MaxLength;
        while (CurrentLine[--SplitPoint] != ' ' && SplitPoint > 0) {}
        if (SplitPoint < 0) SplitPoint = MaxLength; // Did not find a space to split at
        WrappedLines.Add(FText::FromString(CurrentLine.Left(SplitPoint)));
        CurrentLine = CurrentLine.Mid(SplitPoint).TrimStartAndEnd();
    }
    WrappedLines.Add(FText::FromString(CurrentLine));
    return WrappedLines;
}
