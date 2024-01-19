// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"
#include "Components/TextBlock.h"
#include "../NetworkProjectCharacter.h"
#include "Components/Button.h"
#include "NetworkGameInstance.h"
#include "NetGameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Components/HorizontalBox.h"
#include "NetworkPlayerController.h"


void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = GetOwningPlayerPawn<ANetworkProjectCharacter>();
	text_ammo->SetText(FText::AsNumber(0));

	btn_exitSession->OnClicked.AddDynamic(this, &UBattleWidget::OnExitSession);
	btn_Retry->OnClicked.AddDynamic(this, &UBattleWidget::OnRetry);

	text_PlayerList->SetText(FText::FromString(FString(TEXT(""))));

	currentTime = spectatorTime;
}

void UBattleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (player != nullptr)
	{
		int32 ammoCount = player->GetAmmo();
		text_ammo->SetText(FText::AsNumber(ammoCount));

		TArray<APlayerState*> players = GetWorld()->GetGameState<ANetGameStateBase>()->GetMyPlayerList();
		playerList = "";

		for (APlayerState* ps : players)
		{
			AddPlayerList(ps->GetPlayerName(), ps->GetScore());
		}
	}

	// 관전자 모드 유지 시간 체크
	if (bProcessTimer)
	{
		if (currentTime > 0)
		{
			currentTime -= InDeltaTime;
		}
		else
		{
			bProcessTimer = false;
			currentTime = spectatorTime;
			text_respawnTimer->SetVisibility(ESlateVisibility::Hidden);
		}

		text_respawnTimer->SetText(FText::AsNumber((int32)currentTime));
	}
}

void UBattleWidget::PlayHitAnimation()
{
	PlayAnimationForward(hitAnim);
}

void UBattleWidget::ShowButtons()
{
	//btn_exitSession->SetVisibility(ESlateVisibility::Visible);
	hb_menuButtons->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UBattleWidget::AddPlayerList(FString playerName, float score)
{
	playerList.Append(FString::Printf(TEXT("%s : %d\n"), *playerName, (int32)score));
	text_PlayerList->SetText(FText::FromString(playerList));
}


void UBattleWidget::OnExitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitMySession();
}

void UBattleWidget::OnRetry()
{
	ANetworkPlayerController* pc = player->GetController<ANetworkPlayerController>();
	pc->ChangeCharToSpectator();
	pc->SetShowMouseCursor(false);
	pc->SetInputMode(FInputModeGameOnly());
	hb_menuButtons->SetVisibility(ESlateVisibility::Hidden);

	text_respawnTimer->SetVisibility(ESlateVisibility::Visible);
	bProcessTimer = true;
	
}
