// (c) 2025 Sarah Smith


#include "HotSpots/HotSpot.h"

#include "Constants.h"
#include "AdventureTools.h"

#include "Player/AdventurePlayerController.h"
#include "AdventureGameplayTags.h"
#include "Components/SphereComponent.h"
#include "Gameplay/AdventureGameInstance.h"
#include "Gameplay/ManagerProvider.h"
#include "Gameplay/BarkProvider.h"
#include "Internationalization/StringTableRegistry.h"
#include "Items/InventoryItem.h"
#include "Items/AssetActionComponent.h"
#include "Player/ItemManager.h"

#include "Kismet/GameplayStatics.h"

AHotSpot::AHotSpot()
{
	// This is not a "real" sphere - it's not a mesh, its just a collision volume defined by dimensions
	WalkToPoint = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	WalkToPoint->SetupAttachment(RootComponent);
	WalkToPoint->SetSphereRadius(4.0f);
	
	AssetActionComponent = CreateDefaultSubobject<UAssetActionComponent>(TEXT("AssetActionComponent"));
	ManagerProvider = CreateDefaultSubobject<UManagerProvider>(TEXT("ManagerProvider"));
	BarkProvider = CreateDefaultSubobject<UBarkProvider>("BarkProvider");
}

void AHotSpot::BeginPlay()
{
	HotSpotType = IsPickup() ? "PickUp" : "HotSpot";
	HotSpotName = ShortDescription.IsEmpty() ? GetClass()->GetName() : ShortDescription.ToString();
	const UStaticMeshComponent* AStaticMeshComponent = GetStaticMeshComponent();
	if (AStaticMeshComponent && AStaticMeshComponent->GetStaticMesh())
	{
		SetEnableMeshComponent(true);
		Super::OnBeginCursorOver.AddDynamic(this, &AHotSpot::OnBeginCursorOver);
		Super::OnEndCursorOver.AddDynamic(this, &AHotSpot::OnEndCursorOver);
	}
	else
	{
		UE_LOG(LogAdventureGame, Warning, TEXT("%s %s - BeginPlay()- static mesh NOT valid"), *HotSpotType, *HotSpotName);
	}
	Super::BeginPlay();
	WalkToPosition = WalkToPoint->GetComponentLocation();
	if (const APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	{
		WalkToPosition.Z = PlayerPawn->GetActorLocation().Z;
	}

	RegisterForSaveAndLoad();
	DataLoad.ExecuteIfBound(this);
}

void AHotSpot::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RegisteredForSaveAndLoad = false;
	DataSave.ExecuteIfBound(this);
	Super::EndPlay(EndPlayReason);
}

FGameplayTagContainer AHotSpot::GetTags() const
{
	FGameplayTagContainer FTags;
	FTags.AppendTags(HistoryTags);
	if (HotSpotHidden) FTags.AddTag(AdventureGameplayTags::HotSpot_Hidden);
	return FTags;
}

void AHotSpot::SetTags(const FGameplayTagContainer& ATags)
{
	if (ATags.HasTag(AdventureGameplayTags::HotSpot_Hidden))
	{
		Hide();
	}
	else
	{
		Show();
	}
	HistoryTags = ATags.Filter(AdventureGameplayTags::HistoryGameplayTags());
}

void AHotSpot::RegisterForSaveAndLoad()
{
	if (!RegisteredForSaveAndLoad)
	{
		RegisteredForSaveAndLoad = true;
		UAdventureGameInstance *GameInstance = Cast<UAdventureGameInstance>(GetWorld()->GetGameInstance());
		GameInstance->RegisterHotSpotForSaveAndLoad(this);
	}
}

UStoryAction* AHotSpot::ItemDataAssetForAction(EVerbType Verb) const
{
	// TODO - remove this bit of code once the deprecated OnUseSuccessItem and OnGiveSuccessItem are gone
	if (Verb == EVerbType::Use)
	{
		if (UStoryAction *UseItem = OnUseSuccessItem.LoadSynchronous())
		{
			UE_LOG(LogAdventureGame, Warning, TEXT("OnUseSuccessItem is deprecated in %s - use OnItemActivated instead"),
				*(ShortDescription.ToString()));
			return UseItem;
		}
	}
	else if (Verb == EVerbType::Give)
	{
		if (UStoryAction *UseItem = OnGiveSuccessItem.LoadSynchronous())
		{
			UE_LOG(LogAdventureGame, Warning, TEXT("OnGiveSuccessItem is deprecated in %s - use OnItemActivated instead"),
				*(ShortDescription.ToString()));
			return UseItem;
		}
	}
	return OnItemActivated.GetItemDataAssetForAction(Verb);
}

void AHotSpot::OnBeginCursorOver(AActor *TouchedActor)
{
	if (ACommandManager *Command = ManagerProvider->GetCommandManager(this))
	{
		Command->MouseEnterHotSpot(this);
	}
}

void AHotSpot::OnEndCursorOver(AActor *TouchedActor)
{
	if (ACommandManager *Command = ManagerProvider->GetCommandManager(this))
	{
		Command->MouseLeaveHotSpot();
	}
}

EVerbType AHotSpot::CheckForDefaultCommand() const
{
	return EVerbType::WalkTo;
}

void AHotSpot::OnClose_Implementation()
{
	IVerbInteractions::OnClose_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On close"));
	if (auto ItemDataAsset = ItemDataAssetForAction(EVerbType::Close))
	{
		AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
	}
	else
	{
		BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "CloseDefaultText"), this);
	}
}

void AHotSpot::OnOpen_Implementation()
{
	IVerbInteractions::OnOpen_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On open"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "OpenDefaultText"), this);
}

void AHotSpot::OnGive_Implementation()
{
	IVerbInteractions::OnGive_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On give"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "GiveDefaultText"), this);
}

void AHotSpot::OnPickUp_Implementation()
{
	IVerbInteractions::OnPickUp_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Pickup"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PickUpDefaultText"), this);
}

void AHotSpot::OnTalkTo_Implementation()
{
	IVerbInteractions::OnTalkTo_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On talk to"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "TalkToDefaultText"), this);
}

void AHotSpot::OnLookAt_Implementation()
{
	IVerbInteractions::OnLookAt_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On look at"));
	BarkProvider->BarkAndEnd(Description.IsEmpty() ? LOCTABLE(ITEM_STRINGS_KEY, "LookAtDefaultText") : Description, this);
}

void AHotSpot::OnPull_Implementation()
{
	IVerbInteractions::OnPull_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On pull"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PullDefaultText"), this);
}

void AHotSpot::OnPush_Implementation()
{
	IVerbInteractions::OnPush_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On push"));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "PushDefaultText"), this);
}

void AHotSpot::OnUse_Implementation()
{
	// HotSpot Use is mostly relevant for doors.  eg "Use Door"
	// and that is covered in the ADoor::OnUse_Implementation override.
	// If we have an object in the scene we want to use somehow, like a computer
	// terminal or a water-fountain then a custom script would need to be done.
	IVerbInteractions::OnUse_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On use from AHotSpot default implement."));
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "UseDefaultText"), this);
}

void AHotSpot::OnWalkTo_Implementation()
{
	IVerbInteractions::OnWalkTo_Implementation();
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On walk to"));
	if (const ACommandManager *Command = ManagerProvider->GetCommandManager(this))
	{
		if (Command->IsAlreadyAtHotspotClicked())
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("Subject"), ShortDescription);
			BarkProvider->BarkAndEnd(FText::Format(LOCTABLE(ITEM_STRINGS_KEY, "HotSpotWalkAlreadyAt"), Args), this);
		}
		else
		{
			BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "HotSpotWalkArrived"), this);
		}
	}
}

void AHotSpot::OnItemUsed_Implementation()
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Item Used"));
	if (UStoryAction *ItemDataAsset = ItemDataAssetForAction(EVerbType::UseItem))
	{
		const UItemManager *ItemManager = ManagerProvider->GetItemManager(this);
		ensureAlwaysMsgf(ItemManager != nullptr, TEXT("ItemManager is nullptr."));
		// Item was used on this hotspot, and the kind of that item matches the
		// recipe in the ItemDataAsset. 
		if (ItemManager->SourceItemName == ItemDataAsset->SourceItem)
		{
			AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
			return;
		}
	}
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemUsedDefaultText"), this);
}

void AHotSpot::OnItemGiven_Implementation()
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("On Item Given"));
	if (UStoryAction *ItemDataAsset = OnGiveSuccessItem.LoadSynchronous())
	{
		if (const UItemManager *ItemManager = ManagerProvider->GetItemManager(this))
		{
			if (ItemManager->SourceItemName == ItemDataAsset->SourceItem)
			{
				AssetActionComponent->OnItemActionSuccess(ItemDataAsset);
				return;
			}
		}
		else
		{
			UE_LOG(LogAdventureGame, Warning, TEXT("AHotSpot::OnItemUsed_Implementation - APC was invalid!"));
		}
	}
	BarkProvider->BarkAndEnd(LOCTABLE(ITEM_STRINGS_KEY, "ItemGivenDefaultText"), this);
}

AActor *AHotSpot::SpawnAtPlayerLocation(TSubclassOf<AActor> SpawnClass, float Scale, float Lifetime) const
{
	APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	
	FVector PlayerLocation = PlayerPawn->GetTransform().GetLocation();
	FVector SpawnLocation(PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z + ZOffsetForSpawn);
	check(SpawnClass)
	AActor* Item = GetWorld()->SpawnActor<AActor>(
		SpawnClass,
		SpawnLocation,
		SpawnOrientation);
	Item->SetLifeSpan(Lifetime);
	Item->SetActorScale3D(FVector(Scale, Scale, Scale));

	return Item;
}

void AHotSpot::Hide()
{
	if (HotSpotHidden) return;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	HotSpotHidden = true;
}

void AHotSpot::Show()
{
	if (!HotSpotHidden) return;
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	HotSpotHidden = false;
}

void AHotSpot::SetEnableMeshComponent(bool Enabled) const
{
	if (Enabled)
	{
		UStaticMeshComponent* AStaticMeshComponent = GetStaticMeshComponent();
		if (AStaticMeshComponent && AStaticMeshComponent->GetStaticMesh())
		{
			UE_LOG(LogAdventureGame, Verbose, TEXT("%s %s static mesh is valid & enabled."), *HotSpotType, *HotSpotName);
			// StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
			AStaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			// AStaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
			AStaticMeshComponent->SetGenerateOverlapEvents(true);
		}
	}
	else
	{
		UStaticMeshComponent* AStaticMeshComponent = GetStaticMeshComponent();
		if (AStaticMeshComponent && AStaticMeshComponent->GetStaticMesh())
		{
			AStaticMeshComponent->SetCollisionProfileName("NoCollision");
			AStaticMeshComponent->SetVisibility(false);
			UE_LOG(LogAdventureGame, Verbose, TEXT("%s %s - static mesh is valid but disabled"), *HotSpotType, *HotSpotName);
		}
	}
}
