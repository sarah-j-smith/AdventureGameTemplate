// (c) 2025 Sarah Smith

#include "Gameplay/SplitScreenManager.h"

// Sets default values
ASplitScreenManager::ASplitScreenManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASplitScreenManager::BeginPlay()
{
	Super::BeginPlay();

	UWorld* OurWorld = GetWorld();
	if (!OurWorld) return;

	UGameViewportClient *Viewport = OurWorld->GetGameViewport();
	if (!Viewport) return;
	
	FSplitscreenData ScreenLayout;
	auto Screen1 = FPerPlayerSplitscreenData(1.0f, 0.725f, 0, 0);
	ScreenLayout.PlayerData.Add(Screen1);

	// This should force the current split screen type to "none" so we can use that slot
	Viewport->SetForceDisableSplitscreen(true);
	Viewport->SplitscreenInfo[ESplitScreenType::None] = ScreenLayout;
}

// Called every frame
void ASplitScreenManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

