// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetworkGameInstance.h"
#include "Net/UnrealNetwork.h"


void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UNetworkGameInstance* gi = GetGameInstance<UNetworkGameInstance>();

	// ���� �÷��̾ ������ ���� �ν��Ͻ��� ����� ���� �̸��� �����ͼ� ���� RPC�� �̸� ��� ��û�� �Ѵ�.
	if (GetPlayerController() && GetPlayerController()->IsLocalPlayerController() && gi != nullptr)
	{
		FString sessionName = gi->GetSessionName();
		ServerSetMyName(sessionName);
	}
}

void ANetPlayerState::ServerSetMyName_Implementation(const FString& name)
{
	// PlayerState�� �����Ǿ� �ִ� privateName������ ���� �����Ѵ�.
	SetPlayerName(name);

	
}
