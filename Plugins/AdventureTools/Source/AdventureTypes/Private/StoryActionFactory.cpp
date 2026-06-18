// (c) 2026 Storybridge Games


#include "StoryActionFactory.h"

#include "StoryAction.h"

UStoryActionFactory::UStoryActionFactory()
{
	bCreateNew = true;
	SupportedClass = UStoryAction::StaticClass();
}

UObject* UStoryActionFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags,
                                               UObject* Context, FFeedbackContext* Warn)
{
	UStoryAction* StoryAction = NewObject<UStoryAction>(InParent, InClass, InName, Flags | RF_Transactional);
	return StoryAction;
}
