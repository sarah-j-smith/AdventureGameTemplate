// (c) 2025 Sarah Smith


#include "HUD/VerbsUI.h"

#include "AdventureTools.h"
#include "CommonButtonBase.h"
#include "Player/AdventurePlayerController.h"

void UVerbsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	C_Give->OnClicked().AddUObject(this, &UVerbsUI::GiveTriggered);
	C_Open->OnClicked().AddUObject(this, &UVerbsUI::OpenTriggered);
	C_Close->OnClicked().AddUObject(this, &UVerbsUI::CloseTriggered);
	
	C_PickUp->OnClicked().AddUObject(this, &UVerbsUI::PickUpTriggered);
	C_TalkTo->OnClicked().AddUObject(this, &UVerbsUI::TalkToTriggered);
	C_LookAt->OnClicked().AddUObject(this, &UVerbsUI::LookAtTriggered);
	
	C_Use->OnClicked().AddUObject(this, &UVerbsUI::UseTriggered);
	C_Push->OnClicked().AddUObject(this, &UVerbsUI::PushTriggered);
	C_Pull->OnClicked().AddUObject(this, &UVerbsUI::PushTriggered);
	
	UE_LOG(LogAdventureGame, Warning, TEXT("Verbs UI - NativeOnInitialized"));
}

void UVerbsUI::CloseTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Close);
}

void UVerbsUI::OpenTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Open);
}

void UVerbsUI::GiveTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Give);
}

void UVerbsUI::TalkToTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::TalkTo);
}

void UVerbsUI::LookAtTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::LookAt);
}

void UVerbsUI::PickUpTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::PickUp);
}

void UVerbsUI::UseTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Use);
}

void UVerbsUI::PushTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Push);
}

void UVerbsUI::PullTriggered()
{
	ClearActiveButton();
	SetButtonActive(EVerbType::Pull);
}

void UVerbsUI::ClearActiveButton()
{
	UE_LOG(LogAdventureGame, Warning, TEXT(">>>> Clear active button - %s"), *UEnum::GetValueAsString(CurrentVerb));
	switch (CurrentVerb)
	{
	case EVerbType::Close:
		C_Close->SetIsSelected(false);
		break;
	case EVerbType::Open:
		C_Open->SetIsSelected(false);
		break;
	case EVerbType::Give:
		C_Give->SetIsSelected(false);
		break;
	case EVerbType::PickUp:
		C_PickUp->SetIsSelected(false);
		break;
	case EVerbType::TalkTo:
		C_TalkTo->SetIsSelected(false);
		break;
	case EVerbType::LookAt:
		C_LookAt->SetIsSelected(false);
		break;
	case EVerbType::Use:
		C_Use->SetIsSelected(false);
		break;
	case EVerbType::Push:
		C_Push->SetIsSelected(false);
		break;
	case EVerbType::Pull:
		C_Pull->SetIsSelected(false);
		break;
	case EVerbType::WalkTo:
		break;
	default:
		break;
	}
	CurrentVerb = EVerbType::WalkTo;
}

void UVerbsUI::SetButtonActive(EVerbType VerbType)
{
	CurrentVerb = VerbType;
	SetActiveVerb();
}

void UVerbsUI::SetActiveVerb() const
{
	if (OnVerbChanged.IsBound())
	{
		const FString BoundName = OnVerbChanged.GetUObject()->GetName();
		UE_LOG(LogAdventureGame, Display, TEXT("OnVerbChanged - calling %s"), *BoundName);
		OnVerbChanged.Execute(CurrentVerb);
	}
	else
	{
		UE_LOG(LogAdventureGame, Error, TEXT("OnVerbChanged is not bound"));
	}
}
