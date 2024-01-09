// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "NetworkGameInstance.h"
#include "Components/WidgetSwitcher.h"


void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	sl_maxPlayers->SetMinValue(2.0f);
	sl_maxPlayers->SetMaxValue(10.0f);
	sl_maxPlayers->SetStepSize(1.0f);

	// 버튼에 함수 연결(Bind)
	btn_create->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedCreateButton);
	btn_back->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedBackButton);
	btn_MoveToCreatePanel->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedMoveToCreate);
	btn_MoveToFindPanel->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedMoveToFind);
	btn_back2->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedBackButton);
	btn_findSessions->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedFindSessionsButton);

	gi = GetGameInstance<UNetworkGameInstance>();

	// 슬라이더 값이 변경될 때마다 실행될 함수 연결
	sl_maxPlayers->OnValueChanged.AddDynamic(this, &ULoginWidget::OnSliderMoved);
}

void ULoginWidget::OnClickedCreateButton()
{
	if (gi != nullptr)
	{
		gi->CreateSession(eText_roomName->GetText().ToString(), eText_hostName->GetText().ToString(), (int32)sl_maxPlayers->GetValue());
	}
}

void ULoginWidget::OnClickedBackButton()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(0);
}

void ULoginWidget::OnSliderMoved(float value)
{
	text_maxPlayers->SetText(FText::AsNumber((int32)value));
}

void ULoginWidget::OnClickedMoveToCreate()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::OnClickedMoveToFind()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(2);
}

void ULoginWidget::OnClickedFindSessionsButton()
{
	
}
