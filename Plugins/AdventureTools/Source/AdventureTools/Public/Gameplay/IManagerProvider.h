#pragma once

class UItemManager;
class ACommandManager;

struct IManagerProvider
{
	virtual ~IManagerProvider() {}
	
	/// Get a pointer to an Item Manager instance
	virtual UItemManager *GetItemManager(UObject *WorldContextObject) = 0;

	/// Get a pointer to a Command Manager instance
	virtual ACommandManager *GetCommandManager(UObject *WorldContextObject) = 0;
};

Expose_TNameOf(IManagerProvider)