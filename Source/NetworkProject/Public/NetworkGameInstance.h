// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	// 세션 인터페이스 변수
	//TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> sessionInterface;
	IOnlineSessionPtr sessionInterface;

	void CreateSession(FString roomName, FString hostName, int32 playerCount);

private:
	FName mySessionName = FName("Sessac Session");
	
	void OnCreatedSession(FName sessionName, bool bWasSuccessful);
};
