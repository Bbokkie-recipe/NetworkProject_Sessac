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
	// 1. 현재 Possess 중인 Pawn으로부터 Unpossess를 한다.
	APawn* prevPawn = GetPawn();
	UnPossess();
	// 2. GameModeBase에서 Spectator pawn을 생성한다.
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASpectatorPawn* spectator = GetWorld()->SpawnActor<ASpectatorPawn>(gm->SpectatorClass, prevPawn->GetActorTransform(), params);
	// 3. 생성된 spectator pawn에 Possess를 한다.
	Possess(spectator);
	// 4. Unpossess된 pawn을 제거한다.
	prevPawn->Destroy();

	float coolTime = 5.0f;
	FTimerHandle respawnHandler;
	GetWorldTimerManager().SetTimer(respawnHandler, this, &ANetworkPlayerController::ServerRespawnPlayer, coolTime, false);
}


void ANetworkPlayerController::ServerRespawnPlayer_Implementation()
{
	APawn* specPawn = GetPawn();
	UnPossess();
	// 최초 시작 위치에서 부활할 때
	//gm->RestartPlayer(this);
	// 특정 위치에서 부활할 때
	FTransform restartTrans;
	restartTrans.SetLocation(FVector(2200, 700, 210));
	gm->RestartPlayerAtTransform(this, restartTrans);
	specPawn->Destroy();
}