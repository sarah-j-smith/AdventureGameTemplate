// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "Puck.generated.h"

DECLARE_MULTICAST_DELEGATE(FPointAndClickDelegate);

DECLARE_MULTICAST_DELEGATE(FTouchInputDelegate);

UCLASS()
class ADVENTURETOOLS_API APuck : public APawn
{
	GENERATED_BODY()

public:
	APuck();

	// MARK: Inputs
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inputs)
	UInputAction* PointAndClickInput = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inputs)
	UInputAction* TouchInput = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inputs)
	UInputMappingContext* InputMappingContext = nullptr;

	UFUNCTION(BlueprintCallable, Category = Input)
	void HandlePointAndClick(const FInputActionValue &Value);

	UFUNCTION(BlueprintCallable, Category = Input)
	void HandleTouchInput(const FInputActionValue &Value);

	FPointAndClickDelegate PointAndClickDelegate;

	FTouchInputDelegate TouchInputDelegate;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when Pawn is possessed
	virtual void PawnClientRestart() override;

protected:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
