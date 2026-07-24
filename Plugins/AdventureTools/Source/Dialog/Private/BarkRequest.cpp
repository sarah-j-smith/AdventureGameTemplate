#include "BarkRequest.h"
#include "BarkRequest.h"

#include "GameUtils.h"
#include "BarkText.h"
#include "Dialog.h"

FBarkRequest::FBarkRequest(const TArray<FLineInfo>& NewBarkLines, const int32 UID,
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
	: FBarkRequest({FLineInfo(false, NewBarkLine, 
		!!Duration ? Duration : FGameUtils::GetDisplayTimeForString(NewBarkLine.ToString()))}, 
		UID, Color, Position, Duration, IsPlayer)
{
	//
}

void FBarkRequest::GetBarkLines(TArray<FText>& OutBarkLines) const
{
	for (const FLineInfo& Line : BarkLines)
	{
		OutBarkLines.Add(Line.LineText);
	}
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
		return new FBarkRequest(FText::GetEmpty(), UID, Color, nullptr, Duration);
	}
#endif
	bool ShouldCalculateDuration = Duration == 0.0f;
	uint32 BarkLinesNum = 0;
	if (!ShouldCalculateDuration)
	{
		for (const FText& Text : NewBarkLines)
		{
			BarkLinesNum += Text.ToString().Len();
		}
	}
	TArray<FLineInfo> BarkLinesArray;
	for (const FText& Text : NewBarkLines)
	{
		float LineDuration;
		if (ShouldCalculateDuration)
		{
			LineDuration = FGameUtils::GetDisplayTimeForString(Text.ToString());
			Duration += LineDuration;
		}
		else
		{
			LineDuration = Duration * (static_cast<float>(Text.ToString().Len()) / static_cast<float>(BarkLinesNum));
		}
		BarkLinesArray.Add(FLineInfo(false, FText(Text), LineDuration));
	}
	if (HasLongLines(NewBarkLines))
	{
		TArray<FLineInfo> ShortTexts;
		for (const FLineInfo& LineInfo : BarkLinesArray)
		{
			if (LineInfo.LineText.ToString().Len() > BARK_LINE_WIDTH)
			{
				TArray<FText> Wrapped = FGameUtils::WrapTextLinesToMaxCharacters(LineInfo.LineText, BARK_LINE_WIDTH);
				bool IsContinuation = false;
				for (const FText& Text : Wrapped)
				{
					ShortTexts.Add(FLineInfo(IsContinuation, FText(Text), IsContinuation ? 0 : LineInfo.DisplayTime));
					IsContinuation = true;
				}
			}
			else
			{
				ShortTexts.Add(LineInfo);
			}
		}
		return new FBarkRequest(ShortTexts, UID, Color, nullptr, Duration);
	}
	return new FBarkRequest(BarkLinesArray, UID, Color, nullptr, Duration);
}

float FBarkRequest::GetDurationForLine(const int32 LineIndex) const
{
	if (BarkLines[LineIndex].IsContinuation)
	{
		return 0.0f;
	}
	return BarkLines[LineIndex].DisplayTime;
}

FBarkRequest* FBarkRequest::CreatePlayerRequest(const FText& NewBarkLine, const float Duration, const int32 UID)
{
	UE_LOG(LogDialog, Warning, TEXT("CreatePlayerRequest: %s - %s"), 
		(NewBarkLine.IsFromStringTable() ? TEXT("String Table") : TEXT("From String")), *NewBarkLine.ToString());
	const FString NewBarkLineString = NewBarkLine.ToString();
	if (NewBarkLineString.Contains(NEW_LINE_SEPARATOR))
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
	for (const FLineInfo& Line : Request->BarkLines)
	{
		UE_LOG(LogDialog, Warning, TEXT("%s"), *Line.LineText.ToString());
	}
	UE_LOG(LogDialog, Warning, TEXT("Bark request ==================="));
}

bool FBarkRequest::HasLongLines(const TArray<FText>& NewBarkLines)
{
	const auto Result = Algo::FindBy(NewBarkLines, true, [](const FText& Text)
	{
		return Text.ToString().Len() > BARK_LINE_WIDTH;
	});
	return Result != nullptr;
}
