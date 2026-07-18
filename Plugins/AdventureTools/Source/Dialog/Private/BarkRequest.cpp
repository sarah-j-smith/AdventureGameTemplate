#include "BarkRequest.h"

#include "GameUtils.h"
#include "BarkText.h"
#include "Dialog.h"

FBarkRequest::FBarkRequest(const TArray<FText>& NewBarkLines, const int32 UID,
                           const FColor Color, USphereComponent* Position,
                           const float Duration, const bool IsPlayer)
    : NextRequest(nullptr)
      , RequestUID(UID)
      , RequestColor(Color)
      , RequestPosition(Position)
      , RequestDuration(Duration)
      , IsPlayerRequest(IsPlayer)
{
    Algo::Copy(NewBarkLines, BarkLines);
}

FBarkRequest::FBarkRequest(const FText& NewBarkLine, const int32 UID,
                           const FColor Color, USphereComponent* Position,
                           const float Duration, const bool IsPlayer)
    : FBarkRequest(TArray({NewBarkLine}), UID, Color, Position, Duration, IsPlayer)
{
    //
}

FBarkRequest* FBarkRequest::CreatePlayerMultilineRequest(const TArray<FText>& NewBarkLines, float Duration, int32 UID)
{
    if (UID == BARK_UID_NONE)
    {
        UID = FGameUtils::GetUUID();
    }
    const FColor Color = G_Player_Default_Text_Colour.ToFColor(true);
#if WITH_EDITOR
    if (NewBarkLines.IsEmpty())
    {
        UE_LOG(LogDialog, Warning, TEXT("Created player bark with blank text"));
        return new FBarkRequest(NewBarkLines, UID, Color, nullptr, Duration);
    }
#endif
    if (Duration == 0.0f)
    {
        for (const FText& Text : NewBarkLines)
        {
            Duration += FGameUtils::GetDisplayTimeForString(Text.ToString());
        }
    }
    if (HasLongLines(NewBarkLines))
    {
        TArray<FText> ShortTexts;
        uint32 Index = 0;
        TSet<uint32> IsContinuation;
        for (const FText& Text : NewBarkLines)
        {
            if (Text.ToString().Len() > BARK_LINE_WIDTH)
            {
                TArray<FText> Wrapped = FGameUtils::WrapTextLinesToMaxCharacters(Text, BARK_LINE_WIDTH);
                ShortTexts.Append(Wrapped);
                for (int W = 1; W < Wrapped.Num(); W++)
                {
                    IsContinuation.Add(W + Index);
                }
            }
            else
            {
                ShortTexts.Add(Text);
                ++Index;
            }
        }
        FBarkRequest *Request = new FBarkRequest(ShortTexts, UID, Color, nullptr, Duration);
        Request->IsContinuation = IsContinuation;
    }
    return new FBarkRequest(NewBarkLines, UID, Color, nullptr, Duration);
}

float FBarkRequest::GetDurationForLine(const int32 LineIndex) const
{
    if (IsContinuation.Contains(LineIndex)) return 0.0f;
    if (LineIndex < BarkLines.Num() - 1) return BARK_LINE_DELAY;
    const uint32 DiscreteLineCount = BarkLines.Num() - IsContinuation.Num();
    return BARK_LINE_DELAY * DiscreteLineCount;
}

FBarkRequest* FBarkRequest::CreatePlayerRequest(const FText& NewBarkLine, const float Duration, const int32 UID)
{
    if (NewBarkLine.ToString().Contains(NEW_LINE_SEPARATOR))
    {
        const auto Lines = FGameUtils::NewLineSeparatedToArrayText(NewBarkLine);
        return CreatePlayerMultilineRequest(Lines, Duration, UID);
    }
    return CreatePlayerMultilineRequest({NewBarkLine}, Duration, UID);
}

FBarkRequest* FBarkRequest::CreateNPCRequest(const FText& NewBarkLine, const float Duration, USphereComponent* Position,
                                             const FColor Color,
                                             const int32 UID)
{
    FBarkRequest* NPCRequest = CreatePlayerRequest(NewBarkLine, Duration, UID);
    NPCRequest->IsPlayerRequest = false;
    NPCRequest->RequestPosition = Position;
    NPCRequest->RequestColor = Color;
    return NPCRequest;
}

FBarkRequest* FBarkRequest::CreateNPCMultilineRequest(const TArray<FText>& NewBarkLines, float Duration,
                                                      USphereComponent* Position, FColor Color,
                                                      int32 UID)
{
    FBarkRequest* NPCRequest = CreatePlayerMultilineRequest(NewBarkLines, Duration, UID);
    NPCRequest->IsPlayerRequest = false;
    NPCRequest->RequestPosition = Position;
    NPCRequest->RequestColor = Color;
    return NPCRequest;
}

void FBarkRequest::Dump(const FBarkRequest* Request)
{
    UE_LOG(LogDialog, Warning, TEXT("Bark request ==================="));
    UE_LOG(LogDialog, Warning, TEXT("UID: %d - Color: %s - IsPlayer: %hs"),
           Request->RequestUID, *Request->RequestColor.ToHex(), Request->IsPlayerRequest ? "true" : "false");
    for (const FText& Line : Request->BarkLines)
    {
        UE_LOG(LogDialog, Warning, TEXT("%s"), *Line.ToString());
    }
    UE_LOG(LogDialog, Warning, TEXT("Bark request ==================="));
}

bool FBarkRequest::HasLongLines(const TArray<FText>& NewBarkLines)
{
    const auto Result = Algo::FindBy(NewBarkLines, true, [](const FText& Text) { return Text.ToString().Len() > BARK_LINE_WIDTH; });
    return Result != nullptr;
}
