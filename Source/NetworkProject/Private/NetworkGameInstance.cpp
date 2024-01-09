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
		// �����κ��� �̺�Ʈ ���� ���� �Լ��� �����Ѵ�.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnFoundSessions);
	}

	// �� ������ ������ ���� ��û�� �Ѵ�(2�� ����).
	/*FTimerHandle createHandler;

	GetWorld()->GetTimerManager().SetTimer(createHandler, FTimerDelegate::CreateLambda([&]()
		{
			CreateSession(mySessionName, 5);
		}), 2.0f, false);*/

}

// ������ ���� ������ ��û�ϴ� �Լ�
void UNetworkGameInstance::CreateSession(FString roomName, FString hostName, int32 playerCount)
{
	// ���� ���� ���� �ɼ��� �����ϱ� ���� ����ü ����
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bAllowInvites = true;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.NumPublicConnections = playerCount;

	// Ŀ���� ���� ���� �߰��ϱ�
	sessionSettings.Set(FName("Room Name"), roomName, EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("Host Name"), hostName, EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);

	sessionInterface->CreateSession(0, mySessionName, sessionSettings);
	UE_LOG(LogTemp, Warning, TEXT("Try to Create Session..."));
	UE_LOG(LogTemp, Warning, TEXT("Current Platform: %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString());
}

void UNetworkGameInstance::FindSession()
{
	// ���� �˻� ������ �����ϱ�
	sessionSearch = MakeShareable(new FOnlineSessionSearch());
	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = 10;
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);

	// ������ ���� �˻��� ��û�ϱ�
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

// �����κ��� ���� ���(���� ���� ���) �̺�Ʈ �Լ�
void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Name: %s"), *sessionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Session Create: %s"), bWasSuccessful ? *FString("Success!") : *FString("Failed..."));
}

// �����κ��� ���� ���(���� �˻� ���) �̺�Ʈ �Լ�
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
