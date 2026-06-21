// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BarkProvider.generated.h"

class UManagerProvider;
/**
 * 
 */
UCLASS()
class ADVENTURETOOLS_API UBarkProvider : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	UManagerProvider *ManagerProvider;

public:	
	UBarkProvider();
	
	/// Clear any current bark, and immediately bark a message.
	/// Player barks a message and then ends any action sequence
	/// they were doing, unblocked & allowing user input again.
	/// Use when the blueprint event logic should end with a bark.
	/// @param BarkText FText for the player to bark. Should be translatable.
	/// @param WorldContextObject
	UFUNCTION(BlueprintCallable, Category = "PlayerBark", meta = (WorldContext = "WorldContextObject"))
	virtual void BarkAndEnd(FText BarkText, UObject* WorldContextObject);

	/// Player barks a message and continues on any action sequence
	/// they were doing. Use when the event logic should continue
	/// and, user interaction should remain blocked.
	///
	/// Queues the bark to happen after any current barks. 
	/// @param BarkText FText for the player to bark. Should be translatable.
	/// @param WorldContextObject 
	UFUNCTION(BlueprintCallable, Category = "PlayerBark", meta = (WorldContext = "WorldContextObject"))
	virtual void Bark(FText BarkText, UObject* WorldContextObject);

	/// Player barks a series of messages and continues on any action sequence
	/// they were doing. Use when the event logic should continue
	/// and, user interaction should remain blocked.
	///
	/// Queues the bark to happen after any current barks. 
	UFUNCTION(BlueprintCallable, Category = "PlayerBark", meta = (WorldContext = "WorldContextObject"))
	virtual void BarkLines(TArray<FText> BarkTextArray, UObject* WorldContextObject);

	/// Clear any current bark messages being displayed, including
	/// all queued messages.
	UFUNCTION(BlueprintCallable, Category = "PlayerBark", meta = (WorldContext = "WorldContextObject"))
	virtual void ClearBark(UObject* WorldContextObject);
};
