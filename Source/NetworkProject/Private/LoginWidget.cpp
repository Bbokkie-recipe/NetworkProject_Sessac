// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "NetworkGameInstance.h"
#include "Components/WidgetSwitcher.h"
#include "SessionSlotWidget.h"
#include "Components/ScrollBox.h"
#include "Components/ComboBoxString.h"


void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	sl_maxPlayers->SetMinValue(2.0f);
	sl_maxPlayers->SetMaxValue(10.0f);
	sl_maxPlayers->SetStepSize(1.0f);

	text_maxPlayers->SetText(FText::AsNumber(2));

	// 버튼에 함수 연결(Bind)
	btn_create->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedCreateButton);
	btn_back->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedBackButton);
	btn_MoveToCreatePanel->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedMoveToCreate);
	btn_MoveToFindPanel->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedMoveToFind);
	btn_back2->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedBackButton);
	btn_findSessions->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedFindSessionsButton);
	btn_next->OnClicked.AddDynamic(this, &ULoginWidget::SetUserNameAndNext);
	cb_MeshSelect->OnSelectionChanged.AddDynamic(this, &ULoginWidget::SelectMesh);
	cb_ColorSelect->OnSelectionChanged.AddDynamic(this, &ULoginWidget::SelectColor);

	gi = GetGameInstance<UNetworkGameInstance>();

	if (gi != nullptr)
	{
		// 세션 찾기 델리게이트에 함수를 연결한다.
		gi->onCreateSlot.AddDynamic(this, &ULoginWidget::OnSlotCreated);
		gi->onNewSearchComplete.AddDynamic(this, &ULoginWidget::OnClearScrollBox);
		gi->onFindButtonToggle.AddDynamic(this, &ULoginWidget::FindButtonOnOff);
	}

	// 슬라이더 값이 변경될 때마다 실행될 함수 연결
	sl_maxPlayers->OnValueChanged.AddDynamic(this, &ULoginWidget::OnSliderMoved);
	sl_maxPlayers->SetValue(2);
}

void ULoginWidget::OnClickedCreateButton()
{
	if (gi != nullptr)
	{
		gi->CreateMySession(eText_roomName->GetText().ToString(), eText_hostName->GetText().ToString(), (int32)sl_maxPlayers->GetValue());
	}
}

void ULoginWidget::OnClickedBackButton()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::OnSliderMoved(float value)
{
	text_maxPlayers->SetText(FText::AsNumber((int32)value));
}

void ULoginWidget::OnClickedMoveToCreate()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(2);
}

void ULoginWidget::OnClickedMoveToFind()
{
	ws_widgetSwitcher->SetActiveWidgetIndex(3);
	if (gi != nullptr)
	{
		gi->FindMySession();
	}
}

void ULoginWidget::OnClickedFindSessionsButton()
{
	//btn_findSessions->SetIsEnabled(false);
	if (gi != nullptr)
	{
		gi->FindMySession();
	}
}

void ULoginWidget::OnSlotCreated(FString roomName, FString hostName, int32 currentPlayers, int32 maxPlayers, int32 ping, int32 sessionIdx)
{
	// 서버로부터 받은 정보로 슬롯 위젯을 만들어서 추가한다.
	if (slotWidget != nullptr)
	{
		if (USessionSlotWidget* slot_UI = CreateWidget<USessionSlotWidget>(GetWorld(), slotWidget))
		{
			slot_UI->SetSessionInfo(roomName, hostName, currentPlayers, maxPlayers, ping, sessionIdx);
			sb_roomList->AddChild(slot_UI);
		}
	}
}

void ULoginWidget::OnClearScrollBox()
{
	// 기존 슬롯 위젯을 모두 지운다
	sb_roomList->ClearChildren();
}

void ULoginWidget::FindButtonOnOff(bool on)
{
	btn_findSessions->SetIsEnabled(on);
}

void ULoginWidget::SetUserNameAndNext()
{
	// 게임 인스턴스에 입력한 이름을 저장한다.
	gi->SetSessionName(editText_userName->GetText().ToString());

	if (cb_ColorSelect->GetSelectedOption().IsEmpty() || cb_MeshSelect->GetSelectedOption().IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("You have to Selected Options!"))
	}
	else
	{
		ws_widgetSwitcher->SetActiveWidgetIndex(1);
	}
}

void ULoginWidget::SelectMesh(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (gi != nullptr)
	{
		if (SelectedItem.Contains(FString("Manny")))
		{
			gi->meshNum = 0;
		}
		else if (SelectedItem.Equals(FString("Quinn")))
		{
			gi->meshNum = 1;
		}
		else if (SelectedItem == FString("Manequin"))
		{
			gi->meshNum = 2;
		}

		UE_LOG(LogTemp, Warning, TEXT("User Selected Mesh: %s"), *SelectedItem);
	}
}

void ULoginWidget::SelectColor(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (gi != nullptr)
	{
		if (SelectedItem.Contains(FString("Red")))
		{
			gi->meshColor = FColor(255, 0, 0);
		}
		else if (SelectedItem.Equals(FString("Green")))
		{
			gi->meshColor = FColor(0, 255, 0);
		}
		else if (SelectedItem == FString("Blue"))
		{
			gi->meshColor = FColor(0, 0, 255);
		}

		UE_LOG(LogTemp, Warning, TEXT("User Selected Color: %s"), *SelectedItem);
	}
}
