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
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnJoinedSession);
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnDestroyedSession);
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

	// 멀티 플레이를 할 맵으로 이동한다.
	GetWorld()->ServerTravel("/Game/Maps/BattleMap?Listen", true);
}

void UNetworkGameInstance::FindSession()
{
	onFindButtonToggle.Broadcast(false);

	// 세션 검색 조건을 설정하기
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = 10;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);

	// 서버에 세션 검색을 요청하기
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
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
		onNewSearchComplete.Broadcast();

		//for (const FOnlineSessionSearchResult& result : results)
		for (int32 i = 0; i < results.Num(); i++)
		{
			FString foundRoomName;
			results[i].Session.SessionSettings.Get(FName("Room Name"), foundRoomName);
			FString foundHostName;
			results[i].Session.SessionSettings.Get(FName("Host Name"), foundHostName);

			int32 maxPlayerCount = results[i].Session.SessionSettings.NumPublicConnections;
			int32 currentPlayerCount = maxPlayerCount - results[i].Session.NumOpenPublicConnections;

			int32 pingSpeed = results[i].PingInMs;

			// 로그로 확인하기
			UE_LOG(LogTemp, Warning, TEXT("Room Name: %s\nHost Name: %s\nPlayer Count: (%d/%d)\nPing: %d ms\n\n"), *foundRoomName, *foundHostName, currentPlayerCount, maxPlayerCount, pingSpeed);

			// 델리게이트 이벤트 실행하기
			onCreateSlot.Broadcast(foundRoomName, foundHostName, currentPlayerCount, maxPlayerCount, pingSpeed, i);
		}

		onFindButtonToggle.Broadcast(true);
	}
}


void UNetworkGameInstance::JoinSession(int32 roomNumber)
{
	sessionInterface->JoinSession(0, mySessionName, sessionSearch->SearchResults[roomNumber]);
}

void UNetworkGameInstance::ExitSession()
{
	sessionInterface->DestroySession(mySessionName);
}

void UNetworkGameInstance::SetSessionName(FString name)
{
	mySessionName = FName(*name);
}

void UNetworkGameInstance::OnJoinedSession(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogTemp, Warning, TEXT("Joined Session: %s"), *sessionName.ToString());

	switch (result)
	{
	case EOnJoinSessionCompleteResult::Success:
	{
		UE_LOG(LogTemp, Warning, TEXT("Join Success!"));

		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url, NAME_GamePort);
		UE_LOG(LogTemp, Warning, TEXT("url: %s"), *url);

		pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);

		break;
	}
	case EOnJoinSessionCompleteResult::SessionIsFull:
		UE_LOG(LogTemp, Warning, TEXT("Session is full..."));
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		UE_LOG(LogTemp, Warning, TEXT("Session Does Not Exist..."));
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		UE_LOG(LogTemp, Warning, TEXT("Could Not Retrieve Address..."));
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		UE_LOG(LogTemp, Warning, TEXT("You are already in this session..."));
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		UE_LOG(LogTemp, Warning, TEXT("Unknown Error occurred!"));
		break;
	default:
		break;
	}
}

void UNetworkGameInstance::OnDestroyedSession(FName sessionName, bool bWasSuccesful)
{
	UE_LOG(LogTemp, Warning, TEXT("Destory Session: %s"), bWasSuccesful ? *FString("Success!") : *FString("Failed..."));

	if (bWasSuccesful)
	{
		APlayerController* pc = GetWorld()->GetFirstPlayerController();

		if (pc != nullptr)
		{
			pc->ClientTravel(FString("/Game/Maps/LobbyLevel"), ETravelType::TRAVEL_Absolute);
		}
	}
}
