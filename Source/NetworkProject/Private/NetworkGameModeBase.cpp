// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"


AActor* ANetworkGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
    TArray<APlayerStart*> playerStartActors;
    playerStartActors.SetNum(2);

    for (TActorIterator<APlayerStart> playerStart(GetWorld()); playerStart; ++playerStart)
    {
        APlayerStart* ps = *playerStart;
        
        if (ps->ActorHasTag(FName("Red")))
        {
            playerStartActors[0] = ps;
        }
        else
        {
            playerStartActors[1] = ps;
        }
    }
    
    int32 num = index++ % playerStartActors.Num();
    return playerStartActors[num];
}
