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
		// �����κ��� �̺�Ʈ ���� ���� �Լ��� �����Ѵ�.
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetworkGameInstance::OnCreatedSession);

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

// �����κ��� ���� ���(���� ���� ���) �̺�Ʈ �Լ�
void UNetworkGameInstance::OnCreatedSession(FName sessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("Session Name: %s"), *sessionName.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Session Create: %s"), bWasSuccessful ? *FString("Success!") : *FString("Failed..."));
}