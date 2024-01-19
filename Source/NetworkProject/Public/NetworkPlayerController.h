// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetworkPlayerController.generated.h"

/**
 *
 */
UCLASS()
class NETWORKPROJECT_API ANetworkPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;


private:
	class ANetworkGameModeBase* gm;

public:
	UFUNCTION(Server, unreliable)
	void ChangeCharToSpectator();

	UFUNCTION(Server, unreliable)
	void ServerRespawnPlayer();
};
