// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionNotifier.generated.h"

DECLARE_MULTICAST_DELEGATE(FUserInteraction);
DECLARE_MULTICAST_DELEGATE(FPromptListOpenRequest);
DECLARE_MULTICAST_DELEGATE(FPromptListCloseRequest);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTURETOOLS_API UInteractionNotifier : public UActorComponent
{
    GENERATED_BODY()

public:

    /// Subscribe to this event to be alerted when the user has tapped or clicked in
    /// the game UI. Used (for example) to tick through scrolling text instead
    /// of waiting for it to time out.
    /// 
    /// Event that indicates the user clicked or tapped in the game area.
    FUserInteraction UserInteraction;

    /// Subscribe to this event to be alerted when the user has started a
    /// conversation and the list of prompts should be displayed.
    FPromptListOpenRequest PromptListOpenRequest;

    /// Subscribe to this event to be alerted when the user has finished a
    /// conversation and the list of prompts should be hidden.
    FPromptListCloseRequest PromptListCloseRequest;

    void NotifyUserInteraction();

private:
    void StartUserInteractionTimer();

    void ResetUserInteractionTimer();

    UFUNCTION()
    void StopUserInteractionTimer();
    
    FTimerHandle UserInteractionBroadcastTimer;
    
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess))
    bool bUserInteractionActive = false;

    /// When the player taps or clicks do not send another UserInteraction
    /// broadcast event until this amount of time has elapsed.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowPrivateAccess))
    float UserInteractionTime = 0.6f;
};
