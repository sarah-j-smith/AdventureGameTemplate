// (c) 2025 Sarah Smith

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VerbInteractions.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UVerbInteractions : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any object in game that can be interacted with by the player
 */
class ADVENTURETOOLS_API IVerbInteractions
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnGive();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnOpen();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnClose();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnPickUp();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnLookAt();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnTalkTo();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnPush();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnPull();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnUse();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnWalkTo();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnItemUsed();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "VerbInteractions")
	void OnItemGiven();
};
