// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameStateBase.h"
#include "GameFramework/PlayerState.h"

TArray<APlayerState*> ANetGameStateBase::GetMyPlayerList()
{
	// 1. 이름순 정렬
	// 1-1. 람다식으로 사용하는 방식
	//PlayerArray.Sort([](const APlayerState & ps1, const APlayerState & ps2)
	//{
	//	return ps1.GetPlayerName() < ps2.GetPlayerName();
	//});

	// 1-2. static 함수를 연결하는 방식
	//PlayerArray.Sort(ANetGameStateBase::AscendingByString);
	//PlayerArray.Sort(ANetGameStateBase::DescendingByString);

	// 2. 점수순 정렬
	// 2-1. 람다식으로 사용하는 방식
	/*PlayerArray.Sort([](const APlayerState& ps1, const APlayerState& ps2)
		{
			return ps1.GetScore() >= ps2.GetScore();
		});*/
	// 2-2. static 함수를 연결하는 방식
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