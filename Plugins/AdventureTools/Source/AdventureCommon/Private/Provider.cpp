// (c) 2026 Storybridge Games


#include "Provider.h"

TSharedRef<TTypeContainer<>> UProvider::Get()
{
	static auto Instance = MakeShared<TTypeContainer<>>();
	
	return Instance;
}

