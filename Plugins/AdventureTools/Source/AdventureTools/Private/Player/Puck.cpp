// (c) 2025 Sarah Smith


#include "Player/Puck.h"

#include "AdventureTools.h"

#include "EnhancedInputComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APuck::APuck()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("Construct: APuck - receives Player Input clicks"));
}

void APuck::HandlePointAndClick(const FInputActionValue& Value)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("APuck::HandlePointAndClick Broadcast"));
	PointAndClickDelegate.Broadcast();
}

void APuck::HandleTouchInput(const FInputActionValue& Value)
{
	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("APuck::HandlePointAndClick Broadcast"));
	TouchInputDelegate.Broadcast();
}

// Called when the game starts or when spawned
void APuck::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("APuck::BeginPlay"));
}

void APuck::PawnClientRestart()
{
	Super::PawnClientRestart();

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("APuck::PawnClientRestart"));

	if (const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer());
		if (SubSystem)
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			SubSystem->ClearAllMappings();
			// Add each mapping context, along with their priority values. Higher values take priority over lower values.
			SubSystem->AddMappingContext(InputMappingContext, 0);
			UE_LOG(LogAdventureGame, Verbose, TEXT("APuck::PawnClientRestart [re-]bound the input mapping context to: %s"),
				*(PlayerController->GetClass()->GetName()));
		}
	}
}

// Called to bind functionality to input
void APuck::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UE_LOG(LogAdventureGame, VeryVerbose, TEXT("APuck::SetupPlayerInputComponent"));

	if (UEnhancedInputComponent* AInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		AInputComponent->BindAction(PointAndClickInput, ETriggerEvent::Triggered, this,
								   &APuck::HandlePointAndClick);
		AInputComponent->BindAction(TouchInput, ETriggerEvent::Triggered, this,
								   &APuck::HandleTouchInput);
		UE_LOG(LogAdventureGame, Verbose, TEXT("APuck::SetupPlayerInputComponent PointAndClickInput bindings done"));
	}
}

