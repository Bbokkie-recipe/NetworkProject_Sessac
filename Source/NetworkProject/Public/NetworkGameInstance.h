// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetworkGameInstance.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FSearchSessionDele, FString, roomName, FString, hostName, int32, currentPlayers, int32, maxPlayers, int32, ping, int32, sessionIdx);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FServerResponseDele);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindButtonStatus, bool, bIsEnabled);

UCLASS()
class NETWORKPROJECT_API UNetworkGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	// 세션 인터페이스 변수
	//TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> sessionInterface;
	IOnlineSessionPtr sessionInterface;
	
	UPROPERTY(EditAnywhere, BlueprintAssignable, Category="MySettings")
	FSearchSessionDele onCreateSlot;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "MySettings")
	FServerResponseDele onNewSearchComplete;

	UPROPERTY(EditAnywhere, BlueprintAssignable, Category = "MySettings")
	FFindButtonStatus onFindButtonToggle;

	void CreateSession(FString roomName, FString hostName, int32 playerCount);
	void FindSession();
	void JoinSession(int32 roomNumber);
	void ExitSession();

private:
	FName mySessionName = FName("Sessac Session");
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	
	void OnCreatedSession(FName sessionName, bool bWasSuccessful);
	void OnFoundSessions(bool bWasSuccessful);
	void OnJoinedSession(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnDestroyedSession(FName sessionName, bool bWasSuccesful);
};
