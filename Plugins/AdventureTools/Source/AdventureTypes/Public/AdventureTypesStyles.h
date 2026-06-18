#pragma once

class FAdventureTypesStyles final : public FSlateStyleSet
{
public:
	
	FAdventureTypesStyles() : FSlateStyleSet("AdventureTypesStyles") {}
	
	void SetUp();
	
	void TearDown();
	
	static FAdventureTypesStyles &Get();
};
