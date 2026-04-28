// (c) 2025 Sarah Smith


#include "HUD/VerbsUI.h"

#include "AdventureTools.h"
#include "Player/AdventurePlayerController.h"

void UVerbsUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Give->OnClicked.AddDynamic(this, &UVerbsUI::GiveTriggered);
	Open->OnClicked.AddDynamic(this, &UVerbsUI::OpenTriggered);
	Close->OnClicked.AddDynamic(this, &UVerbsUI::CloseTriggered);
	PickUp->OnClicked.AddDynamic(this, &UVerbsUI::PickUpTriggered);
	TalkTo->OnClicked.AddDynamic(this, &UVerbsUI::TalkToTriggered);
	LookAt->OnClicked.AddDynamic(this, &UVerbsUI::LookAtTriggered);
	Use->OnClicked.AddDynamic(this, &UVerbsUI::UseTriggered);
	Push->OnClicked.AddDynamic(this, &UVerbsUI::PushTriggered);
	Pull->OnClicked.AddDynamic(this, &UVerbsUI::PullTriggered);
	
	AssignNormalStyles();
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
	switch (CurrentVerb)
	{
	case EVerbType::Close:
		Close->SetStyle(CloseNormalStyle);
		break;
	case EVerbType::Open:
		Open->SetStyle(OpenNormalStyle);
		break;
	case EVerbType::Give:
		Give->SetStyle(GiveNormalStyle);
		break;
	case EVerbType::PickUp:
		PickUp->SetStyle(PickUpNormalStyle);
		break;
	case EVerbType::TalkTo:
		TalkTo->SetStyle(TalkToNormalStyle);
		break;
	case EVerbType::LookAt:
		LookAt->SetStyle(LookAtNormalStyle);
		break;
	case EVerbType::Use:
		Use->SetStyle(UseNormalStyle);
		break;
	case EVerbType::Push:
		Push->SetStyle(PushNormalStyle);
		break;
	case EVerbType::Pull:
		Pull->SetStyle(PullNormalStyle);
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
	switch (VerbType)
	{
	case EVerbType::Close:
		Close->SetStyle(CloseSelectedStyle);
		break;
	case EVerbType::Open:
		Open->SetStyle(OpenSelectedStyle);
		break;
	case EVerbType::Give:
		Give->SetStyle(GiveSelectedStyle);
		break;
	case EVerbType::PickUp:
		PickUp->SetStyle(PickUpSelectedStyle);
		break;
	case EVerbType::TalkTo:
		TalkTo->SetStyle(TalkToSelectedStyle);
		break;
	case EVerbType::LookAt:
		LookAt->SetStyle(LookAtSelectedStyle);
		break;
	case EVerbType::Use:
		Use->SetStyle(UseSelectedStyle);
		break;
	case EVerbType::Push:
		Push->SetStyle(PushSelectedStyle);
		break;
	case EVerbType::Pull:
		Pull->SetStyle(PullSelectedStyle);
		break;
	case EVerbType::WalkTo:
		break;
	default:
		break;
	}
	CurrentVerb = VerbType;

	SetActiveVerb();
}

void UVerbsUI::AssignNormalStyles()
{
	CloseNormalStyle = FButtonStyle(Close->GetStyle());
	OpenNormalStyle = FButtonStyle(Open->GetStyle());
	GiveNormalStyle = FButtonStyle(Give->GetStyle());
	LookAtNormalStyle = FButtonStyle(LookAt->GetStyle());
	TalkToNormalStyle = FButtonStyle(TalkTo->GetStyle());
	PickUpNormalStyle = FButtonStyle(PickUp->GetStyle());
	UseNormalStyle = FButtonStyle(Use->GetStyle());
	PushNormalStyle = FButtonStyle(Push->GetStyle());
	PullNormalStyle = FButtonStyle(Pull->GetStyle());
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
