// (c) 2025 Sarah Smith


#include "HotSpots/PickUp.h"

#include "AdventureGameplayTags.h"
#include "VerbType.h"

// Sets default values
APickUp::APickUp()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
    SpriteComponent->SetupAttachment(RootComponent);
    SpriteComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
    SpriteComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
    SpriteComponent->SetVisibility(true);

    Pickup = true;
}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
    Super::BeginPlay();
    
}

FGameplayTagContainer APickUp::GetTags() const
{
    FGameplayTagContainer FTags = Super::GetTags();
    if (SpriteHidden) FTags.AddTag(AdventureGameplayTags::HotSpot_SpriteHidden);
    return FTags;
}

void APickUp::SetTags(const FGameplayTagContainer& ATags)
{
    Super::SetTags(ATags);
    if (ATags.HasTag(AdventureGameplayTags::HotSpot_SpriteHidden))
    {
        HideSprite();
    }
    else
    {
        ShowSprite();
    }
}

EVerbType APickUp::CheckForDefaultCommand() const
{
    if (SpriteComponent->IsVisible())
    {
        return EVerbType::PickUp;
    }
    return Super::CheckForDefaultCommand();
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APickUp::HideSprite()
{
    if (SpriteHidden) return;
    SpriteComponent->SetVisibility(false);
    SpriteHidden = true;
}

void APickUp::ShowSprite()
{
    if (!SpriteHidden) return;
    SpriteComponent->SetVisibility(true);
    SpriteHidden = false;
}