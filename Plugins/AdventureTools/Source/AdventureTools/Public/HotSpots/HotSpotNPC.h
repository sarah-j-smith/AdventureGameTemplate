// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DialogInteractable.h"
#include "PositionProvider.h"
#include "DialogComponent.h"
#include "HotSpots/HotSpot.h"
#include "HotSpotNPC.generated.h"

class UPaperFlipbook;
class UPaperFlipbookComponent;

/**
 * An Non-player Character who just stays in the one position, and uses
 * an animation - a FlipBook.
 */
UCLASS()
class DIALOG_API AHotSpotNPC : public AHotSpot, public IDialogInteractable, public IPositionProvider
{
    GENERATED_BODY()

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
public:
    // Sets default values for this actor's properties
    AHotSpotNPC();

    virtual EVerbType CheckForDefaultCommand() const override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void OnConverseWith_Implementation() override;

    virtual void OnTalkTo_Implementation() override;

    UFUNCTION()
    void OnConversationComplete();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NPC")
    USphereComponent* BarkPosition;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NPC")
    UDialogComponent *DialogComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    UPaperFlipbookComponent *FlipbookComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "NPC")
    UPaperFlipbook *DefaultFlipbook;
    
    virtual USphereComponent *GetPosition() const override;

private:
    void StopConversation();
    
    bool IsConversing = false;
    bool ShouldStopConversation = false;
};
