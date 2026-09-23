// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"

#include "IBarkProvider.h"
#include "IManagerProvider.h"

class ADVENTURETOOLS_API FBarkProvider : public IBarkProvider
{
	TSharedRef<IManagerProvider> ManagerProvider;

public:	
	FBarkProvider();
	
	virtual ~FBarkProvider() override;
	
	virtual void BarkAndEnd(FText BarkText, UObject* WorldContextObject) override;

	virtual void Bark(FText BarkText, UObject* WorldContextObject) override;

	virtual void BarkLines(TArray<FText> BarkTextArray, UObject* WorldContextObject) override;

	virtual void ClearBark(UObject* WorldContextObject) override;
};
