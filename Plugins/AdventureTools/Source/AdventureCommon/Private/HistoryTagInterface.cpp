// (c) 2026 Storybridge Games


#include "HistoryTagInterface.h"

#include "AdventureGameplayTags.h"


// Add default functionality here for any IHistoryTagInterface functions that are not pure virtual.
void IHistoryTagInterface::GetHistoryTags(FGameplayTagContainer& HistoryTags)
{
	HistoryTags.AppendTags(GetTagContainer());
}

void IHistoryTagInterface::SetHistoryTags(const FGameplayTagContainer& HistoryTags)
{
#if WITH_EDITOR
	FGameplayTagQueryExpression HistoryTagsOnlyExpr;
	HistoryTagsOnlyExpr.AllTagsMatch().AddTag(AdventureGameplayTags::History);
	const FGameplayTagQuery HistoryTagsOnly = FGameplayTagQuery::BuildQuery(HistoryTagsOnlyExpr);
	ensureAlwaysMsgf(HistoryTags.MatchesQuery(HistoryTagsOnly), TEXT("Expected history tags for query '%s'"),
		*HistoryTags.ToString());
#endif
	
	GetTagContainer().Reset();
	GetTagContainer().AppendTags(HistoryTags);
}

bool IHistoryTagInterface::AddHistoryTag(const FGameplayTag& Tag)
{
	ensureAlwaysMsgf(Tag.MatchesTag(AdventureGameplayTags::History), 
		TEXT("Removing history tag '%s' does not match tag 'History'"), *Tag.ToString());
	if (!GetTagContainer().HasTagExact(Tag))
	{
		GetTagContainer().AddTagFast(Tag);
		return true;
	}
	return false;
}

void IHistoryTagInterface::RemoveHistoryTag(const FGameplayTag& Tag)
{
	ensureAlwaysMsgf(Tag.MatchesTag(AdventureGameplayTags::History), 
		TEXT("Removing history tag '%s' does not match tag 'History'"), *Tag.ToString());
	GetTagContainer().RemoveTag(Tag);
}
