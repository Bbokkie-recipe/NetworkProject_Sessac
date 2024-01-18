// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameStateBase.h"
#include "GameFramework/PlayerState.h"

TArray<APlayerState*> ANetGameStateBase::GetMyPlayerList()
{
	// 1. �̸��� ����
	// 1-1. ���ٽ����� ����ϴ� ���
	//PlayerArray.Sort([](const APlayerState & ps1, const APlayerState & ps2)
	//{
	//	return ps1.GetPlayerName() < ps2.GetPlayerName();
	//});

	// 1-2. static �Լ��� �����ϴ� ���
	//PlayerArray.Sort(ANetGameStateBase::AscendingByString);
	//PlayerArray.Sort(ANetGameStateBase::DescendingByString);

	// 2. ������ ����
	// 2-1. ���ٽ����� ����ϴ� ���
	/*PlayerArray.Sort([](const APlayerState& ps1, const APlayerState& ps2)
		{
			return ps1.GetScore() >= ps2.GetScore();
		});*/
	// 2-2. static �Լ��� �����ϴ� ���
	PlayerArray.Sort(ANetGameStateBase::DescendingByScore);
	

	return PlayerArray;
}

bool ANetGameStateBase::AscendingByString(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetPlayerName() < ps2.GetPlayerName();
}

bool ANetGameStateBase::DescendingByString(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetPlayerName() >= ps2.GetPlayerName();
}

bool ANetGameStateBase::AscendingByScore(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetScore() < ps2.GetScore();
}

bool ANetGameStateBase::DescendingByScore(const APlayerState& ps1, const APlayerState& ps2)
{
	return ps1.GetScore() >= ps2.GetScore();
}