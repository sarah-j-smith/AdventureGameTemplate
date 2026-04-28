// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "HotSpot.h"
#include "PickUp.h"
#include "DoorState.h"

#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API ADoor : public APickUp
{
	GENERATED_BODY()
	
public:
	virtual EVerbType CheckForDefaultCommand() const override;

	virtual FGameplayTagContainer GetTags() const override;

	virtual void SetTags(const FGameplayTagContainer& Tags) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	FName LevelToLoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	FName CurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
	FName DoorLabel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Door)
	EDoorState DoorState = EDoorState::Unknown;

	UFUNCTION(BlueprintCallable, Category = Door)
	bool UnlockDoor();

	UFUNCTION(BlueprintCallable, Category = Door)
	bool LockDoor();

	UFUNCTION(BlueprintCallable, Category = Door)
	bool OpenDoor();

	UFUNCTION(BlueprintCallable, Category = Door)
	bool CloseDoor();

	virtual void OnUse_Implementation() override;

	virtual void OnOpen_Implementation() override;

private:
	void DoorErrors(const FString& Reason);
};
