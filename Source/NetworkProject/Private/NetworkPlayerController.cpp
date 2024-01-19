// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "NetworkGameModeBase.h"
#include "GameFramework/SpectatorPawn.h"


void ANetworkPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bReplicates = true;

	if (HasAuthority())
	{
		gm = GetWorld()->GetAuthGameMode<ANetworkGameModeBase>();
	}
}


void ANetworkPlayerController::ChangeCharToSpectator_Implementation()
{
	// 1. ���� Possess ���� Pawn���κ��� Unpossess�� �Ѵ�.
	APawn* prevPawn = GetPawn();
	UnPossess();
	// 2. GameModeBase���� Spectator pawn�� �����Ѵ�.
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, prevPawn->GetActorTransform(), params);
	// 3. ������ spectator pawn�� Possess�� �Ѵ�.
	Possess(spectator);
	// 4. Unpossess�� pawn�� �����Ѵ�.
	prevPawn->Destroy();

	float coolTime = 5.0f;
	FTimerHandle respawnHandler;
	GetWorldTimerManager().SetTimer(respawnHandler, this, &ANetworkPlayerController::ServerRespawnPlayer, coolTime, false);
}


void ANetworkPlayerController::ServerRespawnPlayer_Implementation()
{
	APawn* specPawn = GetPawn();
	UnPossess();
	// ���� ���� ��ġ���� ��Ȱ�� ��
	//gm->RestartPlayer(this);
	// Ư�� ��ġ���� ��Ȱ�� ��
	FTransform restartTrans;
	restartTrans.SetLocation(FVector(2200, 700, 210));
	gm->RestartPlayerAtTransform(this, restartTrans);
	specPawn->Destroy();
}