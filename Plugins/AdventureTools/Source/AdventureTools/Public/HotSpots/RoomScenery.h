// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "RoomScenery.generated.h"

class AFollowCamera;

UCLASS()
class ADVENTURETOOLS_API ARoomScenery : public APaperSpriteActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomScenery();
	
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/// How fast to move this room scenery when the camera moves. For items
	/// closer, in the near foreground, this should be larger and positive.
	/// For items in the background, eg distant mountains, it should be smaller
	/// and negative.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Parallax")
	float ParallaxSpeed = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parallax")
	TWeakObjectPtr<AFollowCamera> ParallaxCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Parallax")
	FVector2D Origin = FVector2D(1.0f, 1.0f);
};
