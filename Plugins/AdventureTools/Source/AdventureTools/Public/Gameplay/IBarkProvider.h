#pragma once

struct IBarkProvider
{
	virtual ~IBarkProvider() = default;
	
	/// Clear any current bark, and immediately bark a message.
	/// Player barks a message and then ends any action sequence
	/// they were doing, unblocked & allowing user input again.
	/// Use when the blueprint event logic should end with a bark.
	/// @param BarkText FText for the player to bark. Should be translatable.
	/// @param WorldContextObject
	virtual void BarkAndEnd(FText BarkText, UObject* WorldContextObject) = 0;

	/// Player barks a message and continues on any action sequence
	/// they were doing. Use when the event logic should continue
	/// and, user interaction should remain blocked.
	///
	/// Queues the bark to happen after any current barks. 
	/// @param BarkText FText for the player to bark. Should be translatable.
	/// @param WorldContextObject 
	virtual void Bark(FText BarkText, UObject* WorldContextObject) = 0;

	/// Player barks a series of messages and continues on any action sequence
	/// they were doing. Use when the event logic should continue
	/// and, user interaction should remain blocked.
	///
	/// Queues the bark to happen after any current barks. 
	virtual void BarkLines(TArray<FText> BarkTextArray, UObject* WorldContextObject) = 0;

	/// Clear any current bark messages being displayed, including
	/// all queued messages.
	virtual void ClearBark(UObject* WorldContextObject) = 0;
};

Expose_TNameOf(IBarkProvider);
