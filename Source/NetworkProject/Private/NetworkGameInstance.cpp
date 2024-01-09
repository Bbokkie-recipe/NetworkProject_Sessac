// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UNetworkGameInstance::Init()
{
	Super::Init();

	sessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();

	if (sessionInterface != nullptr)
	{
		// 서버로부터 이벤트 값을 받을 함수를 연결한다.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnFoundSessions);
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

void UNetworkGameInstance::FindSession()
{
	// 세션 검색 조건을 설정하기
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = 10;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);

	// 서버에 세션 검색을 요청하기
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

// 서버로부터 들어온 결과(세션 생성 결과) 이벤트 함수
void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Name: %s"), *sessionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Session Create: %s"), bWasSuccessful ? *FString("Success!") : *FString("Failed..."));
}

// 서버로부터 들어온 결과(세션 검색 결과) 이벤트 함수
void UNetworkGameInstance::OnFoundSessions(bool bWasSuccessful)
{
	TArray<FOnlineSessionSearchResult> results = sessionSearch->SearchResults;

	UE_LOG(LogTemp, Warning, TEXT("Find Results: %s"), bWasSuccessful ? *FString("Success!") : *FString("Failed..."));

	if (bWasSuccessful)
	{
		int32 sessionNum = results.Num();
		UE_LOG(LogTemp, Warning, TEXT("Session Count: %d"), results.Num());

		for (const FOnlineSessionSearchResult& result : results)
		{
			FString foundRoomName;
			result.Session.SessionSettings.Get(FName("Room Name"), foundRoomName);
			FString foundHostName;
			result.Session.SessionSettings.Get(FName("Host Name"), foundHostName);
			
			int32 maxPlayerCount = result.Session.SessionSettings.NumPublicConnections;
			int32 currentPlayerCount = maxPlayerCount - result.Session.NumOpenPublicConnections;

			int32 pingSpeed = result.PingInMs;

			UE_LOG(LogTemp, Warning, TEXT("Room Name: %s\nHost Name: %s\nPlayer Count: (%d/%d)\nPing: %d ms\n\n"), *foundRoomName, *foundHostName, currentPlayerCount, maxPlayerCount, pingSpeed);
		}
	}
}
