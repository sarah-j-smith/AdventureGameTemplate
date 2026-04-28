// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HotSpot.h"
#include "PaperSpriteComponent.h"
#include "PickUp.generated.h"

UCLASS()
class ADVENTURETOOLS_API APickUp : public AHotSpot
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APickUp();

    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual FGameplayTagContainer GetTags() const override;

    virtual void SetTags(const FGameplayTagContainer& Tags) override;

    virtual EVerbType CheckForDefaultCommand() const override;
    
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    /// Show the sprite, the visible 2D graphic for this hotspot. Some hotspots
    /// may not have a sprite, in which case this function will do nothing.
    UFUNCTION(BlueprintCallable, Category = "Player Actions")
    void HideSprite();
	
    /// Show the sprite, the visible 2D graphic for this hotspot. Some hotspots
    /// may not have a sprite, in which case this function will do nothing.
    UFUNCTION(BlueprintCallable, Category = "Player Actions")
    void ShowSprite();
    
    /// A sprite component that <b>may</b> be displayed on this hotspot. Can be null if
    /// the hotspot is just representing a spot on the background.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PickUp")
    UPaperSpriteComponent *SpriteComponent;
    
private:
    bool SpriteHidden = false;
};
