// (c) 2026 Storybridge Games


#include "AdvGameDataFunctionLibrary.h"

#include "AdventureCommon.h"
#include "GameplayTagContainer.h"
#include "HistoryTagInterface.h"

void UAdvGameDataFunctionLibrary::TagStateCheck(FGameplayTagContainer HistorySourceTags,
                                                FGameplayTag CheckTag,
                                                ETagQueryResultType& TagQueryResult,
                                                ETagQuerySense TagQuerySense)
{
	FGameplayTagQueryExpression CompleteQueryExpr;
	switch (TagQuerySense)
	{
	case ETagQuerySense::TagMustBePresent:
		CompleteQueryExpr.AllTagsMatch().AddTag(CheckTag);
		break;
	case ETagQuerySense::TagMustNotBePresent:
		CompleteQueryExpr.NoTagsMatch().AddTag(CheckTag);
		break;
	}
	FGameplayTagQuery Query = FGameplayTagQuery::BuildQuery(CompleteQueryExpr);

	UE_LOG(LogAdventureCommon, VeryVerbose, TEXT("TagStateCheck for %s - %s"), *CheckTag.ToString(),
	       *Query.GetDescription());

	if (Query.Matches(HistorySourceTags))
	{
		TagQueryResult = ETagQueryResultType::Continue;
	}
	else
	{
		TagQueryResult = ETagQueryResultType::Blocked;
	}
}

void UAdvGameDataFunctionLibrary::HistoryTagGate(TScriptInterface<IHistoryTagInterface> HistoryObject,
                                                 FGameplayTag CheckTag, ETagQueryResultType& TagQueryResult)
{
	if (HistoryObject->AddHistoryTag(CheckTag))
	{
		TagQueryResult = ETagQueryResultType::Continue;
		return;
	}
	TagQueryResult = ETagQueryResultType::Blocked;
}
