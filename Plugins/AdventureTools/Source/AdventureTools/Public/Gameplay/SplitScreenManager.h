// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplitScreenManager.generated.h"

UCLASS()
class ADVENTURETOOLS_API ASplitScreenManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplitScreenManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
