// (c) 2026 Storybridge Games

#pragma once

#include "IManagerProvider.h"

class UItemManager;
class ACommandManager;

class ADVENTURETOOLS_API FManagerProvider : public IManagerProvider
{
public:
	/// Get a pointer to an Item Manager instance
	virtual UItemManager *GetItemManager(UObject *WorldContextObject) override;
    
	/// Get a pointer to a Command Manager instance
	virtual ACommandManager *GetCommandManager(UObject *WorldContextObject) override;
};
