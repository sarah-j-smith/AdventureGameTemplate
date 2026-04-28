#include "VerbType.h"

#include "Constants.h"
#include "Internationalization/StringTableRegistry.h"

/**
 * Get a text to use to describe the verb. Note that these cannot
 * generally just be concatenated with nouns due to i18n requirements,
 * for example:
 * 
 * English:
 *     give the gift to the woman
 *     open the door
 *     
 * German:
 *     gib der Frau das Geschenk
 *     öffne die Tür
 * 
 * @param Verb Verb to 
 * @return 
 */
FText VerbGetDescriptiveString(EVerbType Verb)
{
	switch (Verb)
	{
	case EVerbType::Close:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Close");
	case EVerbType::Open:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Open");
	case EVerbType::Give:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Give");
	case EVerbType::PickUp:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Pick_up");
	case EVerbType::TalkTo:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Talk_to");
	case EVerbType::LookAt:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Look_at");
	case EVerbType::Use:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Use");
	case EVerbType::Push:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Push");
	case EVerbType::Pull:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Pull");
	case EVerbType::WalkTo:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Walk_to");
	case EVerbType::UseItem:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Use_item");
	case EVerbType::GiveItem:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Give");
	default:
		return LOCTABLE(ITEM_DESCRIPTIONS_KEY, "Walk_to");
	}
}
