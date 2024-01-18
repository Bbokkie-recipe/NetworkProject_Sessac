// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleWidget.h"
#include "Components/TextBlock.h"
#include "../NetworkProjectCharacter.h"
#include "Components/Button.h"
#include "NetworkGameInstance.h"
#include "NetGameStateBase.h"
#include "GameFramework/PlayerState.h"


void UBattleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	player = GetOwningPlayerPawn<ANetworkProjectCharacter>();
	text_ammo->SetText(FText::AsNumber(0));

	btn_exitSession->OnClicked.AddDynamic(this, &UBattleWidget::OnExitSession);

	text_PlayerList->SetText(FText::FromString(FString(TEXT(""))));
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
}

void UBattleWidget::PlayHitAnimation()
{
	PlayAnimationForward(hitAnim);
}

void UBattleWidget::ShowButtons()
{
	btn_exitSession->SetVisibility(ESlateVisibility::Visible);
}

void UBattleWidget::AddPlayerList(FString playerName, float score)
{
	playerList.Append(FString::Printf(TEXT("%s : %d\n"), *playerName, (int32)score));
	text_PlayerList->SetText(FText::FromString(playerList));
}


void UBattleWidget::OnExitSession()
{
	GetGameInstance<UNetworkGameInstance>()->ExitSession();
}
