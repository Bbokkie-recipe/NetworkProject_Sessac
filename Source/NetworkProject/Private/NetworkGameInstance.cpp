// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


void UNetworkGameInstance::Init()
{
	Super::Init();

	sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();

	if (sessionInterface != nullptr)
	{
		// 서버로부터 이벤트 값을 받을 함수를 연결한다.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);

	}

	// 내 세션을 서버에 생성 요청을 한다(2초 지연).
	/*FTimerHandle createHandler;

	GetWorld()->GetTimerManager().SetTimer(createHandler, FTimerDelegate::CreateLambda([&]()
		{
			CreateSession(mySessionName, 5);
		}), 2.0f, false);*/
	
}

// 서버에 세션 생성을 요청하는 함수
void UNetworkGameInstance::CreateSession(FString roomName, FString hostName, int32 playerCount)
{
	// 서버 생성 시의 옵션을 설정하기 위한 구조체 변수
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bAllowInvites = true;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.NumPublicConnections = playerCount;
	
	// 커스텀 설정 값을 추가하기
	sessionSettings.Set(FName("Room Name"), roomName, EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("Host Name"), hostName, EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);

	sessionInterface->CreateSession(0, mySessionName, sessionSettings);
	UE_LOG(LogTemp, Warning, TEXT("Try to Create Session..."));
	UE_LOG(LogTemp, Warning, TEXT("Current Platform: %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
}

// 서버로부터 들어온 결과(세션 생성 결과) 이벤트 함수
void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Name: %s"), *sessionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Session Create: %s"), bWasSuccessful ? *FString("Success!") : *FString("Failed..."));
}