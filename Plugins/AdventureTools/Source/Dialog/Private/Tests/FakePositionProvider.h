// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "PositionProvider.h"
#include "UObject/Object.h"

#include "FakePositionProvider.generated.h"

UCLASS()
class UFakePositionProvider : public UObject, public IPositionProvider
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	USphereComponent *Sphere;
	
	UFakePositionProvider();
	
	virtual USphereComponent* GetPosition() const override;
};

