#pragma once

#include "Constants.h"

class USphereComponent;

class DIALOG_API FBarkRequest
{
public:
	struct FLineInfo
	{
		bool IsContinuation;
		FText LineText;
		uint32 DisplayTime;
	};
	
    explicit FBarkRequest(const TArray<FLineInfo>& NewBarkLines, int32 UID = BARK_UID_NONE,
                          FColor Color = G_NPC_Default_Text_Colour.ToFColor(true),
                          USphereComponent* Position = nullptr, float Duration = 0.0f, bool IsPlayer = true);

    explicit FBarkRequest(const FText& NewBarkLine, int32 UID = BARK_UID_NONE,
                          FColor Color = G_NPC_Default_Text_Colour.ToFColor(true),
                          USphereComponent* Position = nullptr, float Duration = 0.0f, bool IsPlayer = true);

    void GetBarkLines(TArray<FText>&OutBarkLines) const;

    int32 GetLineCount() const { return BarkLines.Num(); }

    int32 GetUID() const { return RequestUID; }

    FColor GetColor() const { return RequestColor; }

    USphereComponent* GetPosition() const { return RequestPosition; }

    float GetDuration() const { return RequestDuration; }

    bool IsPlayer() const { return IsPlayerRequest; }

    float GetDurationForLine(int32 LineIndex) const;

    FBarkRequest *NextRequest;

    /**
     * Create a bark text request for the Player. Use this for lines which may have newline
     * characters, for example lines out of text entries in regular string tables. For data table
     * text which is already in multiple lines, like conversations, use CreatePlayerMultilineRequest.
     * The position will be set to the Player bark location sphere, when the request is barked.
     * The text color will be set to the player default (white).
     * @see CreatePlayerMultilineRequest
     * @param NewBarkLine Line of text possibly with newline <code>'\n'</code> characters
     * @param Duration How long to display for, or 0.0 for a time based on length
     * @param UID Identifier for this request, or leave as <code>BARK_UID_NONE</code> ( -1 ) to have one assigned
     * @return Complete request ready to bark
     */
    static FBarkRequest* CreatePlayerRequest(const FText& NewBarkLine, float Duration = 0.0f, int32 UID = BARK_UID_NONE);

    /**
     * Create a bark text request for the Player. Use this for data table
     * text which is already in multiple lines, like conversations. The text should <b>not</b>
     * have newline characters in it. These will show as literals in the text.
     * The position will be set to the Player bark location sphere, when the request is barked.
     * The text color will be set to the player default (white).
     * @see CreatePlayerMultilineRequest
     * @param NewBarkLines Lines of text (must not have newline <code>'\n'</code> characters)
     * @param Duration How long to display for, or 0.0 for a time based on length
     * @param UID Identifier for this request, or leave as <code>BARK_UID_NONE</code> ( -1 ) to have one assigned
     * @return Complete request ready to bark
     */
    static FBarkRequest* CreatePlayerMultilineRequest(const TArray<FText>& NewBarkLines, float Duration = 0.0f,
                                                      int32 UID = BARK_UID_NONE);

    /**
     * Create a bark text request for an NPC. Use this for lines which may have newline
     * characters, for example lines out of text entries in data tables. For data table
     * text which is already in multiple lines, use CreateNPCMultilineRequest.
     * @see CreateNPCMultilineRequest
     * @param NewBarkLine Line of text possibly with newline <code>'\n'</code> characters
     * @param Duration How long to display for, or if 0.0 will be set to a time based on length
     * @param Position Position of the NPC via a sphere component reference
     * @param Color Color of bark text, or defaults to NPC color
     * @param UID Identifier for this request, or leave as BARK_UID_NONE to have one assigned
     * @return Complete request ready to bark
     */
    static FBarkRequest* CreateNPCRequest(const FText& NewBarkLine, float Duration,
                                          USphereComponent* Position,
                                          FColor Color = G_NPC_Default_Text_Colour.ToFColor(true),
                                          int32 UID = BARK_UID_NONE);

    /**
     * Create a bark text request for an NPC. Use this for data table
     * text which is already in multiple lines, like conversations. The text <b>must</b>
     * not have newline characters in it. These will be removed, ignored and a warning printed.
     * @see CreateNPCRequest
     * @param NewBarkLines Lines of text (must not have newline <code>'\n'</code> characters)
     * @param Duration How long to display for, or 0.0 for a time based on length
     * @param Color Color of bark text, or defaults to NPC color
     * @param Position Position of the NPC via a sphere component reference
     * @param UID Identifier for this request, or leave as BARK_UID_NONE to have one assigned
     * @return Complete request ready to bark
     */
    static FBarkRequest* CreateNPCMultilineRequest(const TArray<FText>& NewBarkLines, float Duration = 0.0f,
                                                   USphereComponent* Position = nullptr,
                                                   FColor Color = G_NPC_Default_Text_Colour.ToFColor(true),
                                                   int32 UID = BARK_UID_NONE);

    static void Dump(const FBarkRequest* Request);

    static bool HasLongLines(const TArray<FText>& NewBarkLines);

private:
    TArray<FLineInfo> BarkLines;
    int32 RequestUID = 0.0;
    FColor RequestColor;
    USphereComponent* RequestPosition = nullptr;
    float RequestDuration = 0.0f;
    bool IsPlayerRequest = false;
    mutable int TotalLineCount = 0;
    mutable int TotalCharacterCount = 0;
};
