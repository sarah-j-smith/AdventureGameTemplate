// (c) 2026 Storybridge Games


#include "BarkTextSUT.h"

#include <functional>

#include "BarkLine.h"
#include "FakePositionProvider.h"
#include "GameFramework/Character.h"

UBarkTextSUT::UBarkTextSUT(const FObjectInitializer &ObjectInitializer)
	: UUserWidget(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UBarkText> BlueprintActor(
		TEXT("/AdventureTools/PointAndClick/Blueprints/Dialog/BP_BarkHolder.BP_BarkHolder_C"));
	if (BlueprintActor.Succeeded())
	{
		BarkTextClass = BlueprintActor.Class;
	}
}

int UBarkTextSUT::TickUntil(float DeltaTime, float maxTime, std::function<bool(UBarkText*)> StopCondition)
{
	float ElapsedTime = 0.0f;
	while (!StopCondition(SUT))
	{
		if (ElapsedTime > maxTime)
		{
			return TIMED_OUT;
		}
		ElapsedTime += DeltaTime;
		SUT->DoTick(DeltaTime);
	}
	return TARGET_REACHED;
}

UBarkText* UBarkTextSUT::CreateBarkText(UWorld* World)
{
	// /Script/UMGEditor.WidgetBlueprint'/AdventureTools/PointAndClick/Blueprints/Dialog/BP_BarkHolder.BP_BarkHolder'
	SUT = CreateWidget<UBarkText>(World, BarkTextClass);
	
	APlayerController* PlayerController = Cast<APlayerController>(World->SpawnActor(APlayerController::StaticClass()));
	ACharacter *PlayerCharacter = Cast<ACharacter>(World->SpawnActor(ACharacter::StaticClass()));
	
	FString ErrorMessage;
	// World->GetGameInstance()->CreateLocalPlayer(1000, ErrorMessage, false);
	ULocalPlayer* LocalPlayer = NewObject<ULocalPlayer>(GEngine);
	PlayerController->SetPlayer(LocalPlayer);
	PlayerController->SetPawn(PlayerCharacter);
	SUT->SetOwningPlayer(PlayerController);
			
	UFakePositionProvider *FakePositionProvider = NewObject<UFakePositionProvider>(World, "FakePositionProvider");
	SUT->SetPositionProvider(FakePositionProvider);
	
	return SUT;
}
