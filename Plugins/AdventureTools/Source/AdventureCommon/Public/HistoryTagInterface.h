// (c) 2026 Storybridge Games

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "HistoryTagInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHistoryTagInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ADVENTURECOMMON_API IHistoryTagInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/// Remove all contents of <code>HistoryTags</code> and then copy of all the tags on 
	/// this object into <code>HistoryTags</code>. Can be re-implemented/overridden in 
	/// sub-classes to retrieve the object state and store as a tag, eg if the the door 
	/// state is <code>Opened</code> then set the tag to <code>History.Triggered.Open</code>.
	/// @param HistoryTags reference to a container to <b>clear</b> and fill up with all the tags from this object.
	virtual void GetHistoryTags(FGameplayTagContainer &HistoryTags);
	
	/// Set this object, eg a <code>HotSpot</code> or <code>InventoryItem</code> to have the
	/// exact history specified in the given <code>HistoryTags</code>. Can be re-implemented/overridden in 
	/// sub-classes to modify the object to reflect the state, eg if the tag is
	/// <code>History.Triggered.Open</code> then set the door state to <code>Opened</code>.
	/// Warning: the default method simply sets all the relevant tags, without making any
	/// changes to the actual object.
	/// @param HistoryTags this is the tags that will replace this objects history tag collection
	virtual void SetHistoryTags(const FGameplayTagContainer &HistoryTags);
	
	/// If the given <code>Tag</code it wasn't present, add it and return true, otherwise  
	/// do nothing, and return false. Good for implementing gates, such as when you don't want something to
	/// occur if the tag is present.
	/// @param Tag to check, and if not present add.
	/// @return true if the tag was not present and was added; false otherwise.
	virtual bool AddHistoryTag(UPARAM(meta=(Category="History")) const FGameplayTag& Tag);
	
	virtual void RemoveHistoryTag(UPARAM(meta=(Category="History")) const FGameplayTag& Tag);

protected:
	virtual FGameplayTagContainer &GetTagContainer() = 0;
};
