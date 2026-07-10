// (c) 2026 Storybridge Games


#include "HotSpots/RoomScenery.h"

#include "Player/FollowCamera.h"
#include "PaperSpriteComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ARoomScenery::ARoomScenery()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ARoomScenery::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	UPaperSpriteComponent *SpriteComponent = GetRenderComponent();
	SpriteComponent->SetMobility(EComponentMobility::Movable);
	SpriteComponent->SetCanEverAffectNavigation(false);
	SpriteComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ARoomScenery::BeginPlay()
{
	Super::BeginPlay();
	
	if (ParallaxSpeed != 0.0f && ParallaxCamera.Get() == nullptr)
	{
		Origin = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		ParallaxCamera = Cast<AFollowCamera>(
			UGameplayStatics::GetActorOfClass(this, AFollowCamera::StaticClass()));
	}
}

// Called every frame
void ARoomScenery::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ParallaxSpeed != 0.0f && ParallaxCamera != nullptr)
	{
		SetActorLocation(FVector(Origin.X + ParallaxCamera->GetActorLocation().X * ParallaxSpeed,
								 Origin.Y + ParallaxCamera->GetActorLocation().Y * ParallaxSpeed, GetActorLocation().Z));
	}
}

