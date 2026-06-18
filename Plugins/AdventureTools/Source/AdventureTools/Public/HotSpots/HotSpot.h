// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "DescribableItem.h"
#include "GameplayTagContainer.h"

#include "Engine/StaticMeshActor.h"
#include "Gameplay/VerbInteractions.h"
#include "WalkDirection.h"
#include "ItemDataList.h"

#include "StoryAction.h"
#include "Items/ItemManagerProvider.h"
#include "Player/BarkProvider.h"

#include "HotSpot.generated.h"

enum class EVerbType : uint8;
class USphereComponent;
class UAssetActionComponent;

DECLARE_DYNAMIC_DELEGATE_OneParam(FHotSpotDataSave, AHotSpot *, HotSpot);
DECLARE_DYNAMIC_DELEGATE_OneParam(FHotSpotDataLoad, AHotSpot *, HotSpot);

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API AHotSpot : public AStaticMeshActor, public IVerbInteractions,
		public IItemManagerProvider, public IBarkProvider, public IDescribableItem
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	AHotSpot();
		
	//////////////////////////////////
	///
	/// IDescribableItem
	///
	
	virtual FText GetShortDescription() const override { return ShortDescription; };

	virtual FText GetLongDescription() const override { return Description; };

	virtual FName GetItemKind() const override { return NAME_None; }
	
	//////////////////////////////////
	///
	/// SAVE AND LOAD
	///

	FHotSpotDataLoad DataLoad;
	FHotSpotDataSave DataSave;

	virtual FGameplayTagContainer GetTags() const;
	virtual void SetTags(const FGameplayTagContainer& Tags);

private:
	bool RegisteredForSaveAndLoad = false;
	void RegisterForSaveAndLoad();

public:
	//////////////////////////////////
	///
	/// HOTSPOT PROPERTIES
	///

	/// Various tags for the state and past actions done on this hotspot.
	/// For example can set "History.Triggered.LookAt" to enforce that a score increment
	/// is only given once, the first time the item is looked at.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Scripting", meta = (Categories = "History"))
	FGameplayTagContainer HistoryTags;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HotSpot")
	USphereComponent* WalkToPoint;

	/// A long description of this hotspot. When the player does a "LookAt" by default
	/// unless overridden in a blueprint, this description will be shown. Should usually
	/// be full sentence case as in "This knife looks sharp."
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotSpot")
	FText Description;

	/// A short description of this hotspot, suitable for insertion into text templates
	/// with other text around it. Should not be sentence case. Example: "sharp knife".
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotSpot")
	FText ShortDescription;

	/// Position that the player will walk to when going to this hotspot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotSpot")
	FVector WalkToPosition;

	/// Direction the player will face on arrival at the hotspot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotSpot")
	EWalkDirection FacingDirection;

	/// Data Asset for determining results of the player interacting via the Use verb with this hotspot.
	/// This should be an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
	/// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
	/// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
	/// @deprecated Use OnItemActivated instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	TSoftObjectPtr<UStoryAction> OnUseSuccessItem;

	/// Data Asset for determining results of the player interacting via the Give verb with this hotspot.
	/// This should be an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
	/// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
	/// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
	/// @deprecated Use OnItemActivated instead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	TSoftObjectPtr<UStoryAction> OnGiveSuccessItem;
	
	/// Data Asset for determining results of successfully activating this Hotspot. This should be
	/// an <b>instance</b> of the <code>ItemDataAsset</code> sub-class, not the
	/// class itself. Right-click in the content drawer, and choose <i>Miscellaneous > Data Asset</i>
	/// then choose one of the <code>ItemDataAsset</code> sub-classes to create an instance.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemHandling")
	FItemDataList OnItemActivated;

private:
	UStoryAction* ItemDataAssetForAction(EVerbType Verb) const;
	
	//////////////////////////////////
	///
	/// USER INPUT EVENTS
	///

public:
	UFUNCTION(BlueprintCallable, Category = "HotSpot")
	void OnBeginCursorOver(AActor *TouchedActor);

	UFUNCTION(BlueprintCallable, Category = "HotSpot")
	void OnEndCursorOver(AActor *TouchedActor);

	//////////////////////////////////
	///
	/// VERB TRIGGER EVENTS
	///

	UFUNCTION(BlueprintCallable, Category = "HotSpot")
	virtual EVerbType CheckForDefaultCommand() const;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnClose_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnOpen_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnGive_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnPickUp_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnTalkTo_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnLookAt_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnPull_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnPush_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnUse_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnWalkTo_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnItemUsed_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "VerbInteractions")
	virtual void OnItemGiven_Implementation() override;

	//////////////////////////////////
	///
	/// PLAYER ACTIONS
	///

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Actions")
	UAssetActionComponent* AssetActionComponent;
	
	/// True if this HotSpot is a <b>Pickup</b>, which has a sprite component
	/// for displaying an in-game appearance - or is it just a hotspot with
	/// a clickable mesh?
	/// @returns true if this has a valid `USpriteComponent`.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	bool IsPickup() const
	{
		return Pickup;
	}
	
	/// How far above the player character (closer to the camera) should objects spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Actions")
	float ZOffsetForSpawn = 5.0;
	
	/// How far above the player character (closer to the camera) should objects spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Actions")
	FRotator SpawnOrientation = FRotator(0.0, 0.0, -90.0);
	
	/// Spawn an actor of the given class on top of the player character.
	/// Optionally specify a scale and how long it should appear before
	/// being destroyed automatically. Intended for item interactions,
	/// where the actor is destroyed on adding to the inventory.
	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	AActor *SpawnAtPlayerLocation(TSubclassOf<AActor> SpawnClass, float Scale = 1.0f, float Lifetime = 2.0f) const;

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	void Hide();

	UFUNCTION(BlueprintCallable, Category = "Player Actions")
	void Show();

	void SetEnableMeshComponent(bool Enabled) const;

protected:
	// Used in debug messages only - not unique.
	FString HotSpotType = "HotSpot";
	FString HotSpotName = "HotSpot";
	
	bool HotSpotHidden = false;
	bool Pickup = false;
};
