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

FBarkRequest *UBarkTextSUT::SetupTestData(const TArray<FText>& BarkTextArray)
{
	FBarkRequest *BarkRequest = FBarkRequest::CreatePlayerMultilineRequest(BarkTextArray);
		
	SUT->AddBarkRequest(BarkRequest);
	return BarkRequest;
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
	
	SUT->BarkRequestCompleteDelegate.AddLambda([this](int UID)
	{
		CompletedUID = UID;
		FinishedReason = EBarkRequestFinishedReason::Timeout;
	});
	SUT->BarkRequestInterruptedDelegate.AddLambda([this](int UID)
	{
		CompletedUID = UID; 
		FinishedReason = EBarkRequestFinishedReason::Interruption;
	});
	
	return SUT;
}

int UBarkTextSUT::TickUntilChildCount(int Count, float MaxTime)
{
	int ResultCode = TickUntil(1.0f, MaxTime, [Count](UBarkText *BarkTextArg)
	{
		return BarkTextArg->BarkContainer->GetChildrenCount() == Count;
	});
	return ResultCode;
}

int UBarkTextSUT::TickUntilTextExists(const FText& SearchText, float MaxTime)
{
	return TickUntil(1.0f, MaxTime, [SearchText](UBarkText *BarkTextArg)
	{
		TArray<UWidget*> ChildrenNodes = BarkTextArg->BarkContainer->GetAllChildren();
		for (UWidget* Child : ChildrenNodes)
		{
			UBarkLine *BarkLine = Cast<UBarkLine>(Child);
			if (BarkLine->Text->GetText().ToString() == SearchText.ToString()) return true;
		}
		return false;
	});
}
