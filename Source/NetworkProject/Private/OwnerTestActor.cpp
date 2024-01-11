// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerTestActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../NetworkProjectCharacter.h"
#include "EngineUtils.h"

AOwnerTestActor::AOwnerTestActor()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootComp);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);

	bReplicates = true;
	SetReplicateMovement(true);
}

void AOwnerTestActor::BeginPlay()
{
	Super::BeginPlay();
	
	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();

}

void AOwnerTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//PrintInfoLog();
	

	// 액터 소유권 변경
	AActor* owningActor = GetNearestDistance();
	//UE_LOG(LogTemp, Warning, TEXT("Nearest is %s"), owningActor != nullptr ? *owningActor->GetActorNameOrLabel() : *FString("No Owner"));

	DrawDebugSphere(GetWorld(), GetActorLocation(), maxDistance, 30, FColor::Green, false, 0, 0, 1.0f);
}

void AOwnerTestActor::PrintInfoLog()
{
	FString localRoleString = UEnum::GetValueAsString<ENetRole>(localRole);
	FString remoteRoleString = UEnum::GetValueAsString<ENetRole>(remoteRole);
	FString ownerString = GetOwner() == nullptr ? *FString("No Owner") : *GetOwner()->GetActorNameOrLabel();
	FString connectionString = GetNetConnection() == nullptr ? *FString("Invalid Connection") : *FString("Valid Connection");

	FString printString = FString::Printf(TEXT("Local Role: %s\nRemote Role: %s\nOwner: %s\nNet Connection: %s\n"), *localRoleString, *remoteRoleString, *ownerString, *connectionString);

	DrawDebugString(GetWorld(), GetActorLocation(), printString, nullptr, FColor::Yellow, 0, true, 1.0f);
}

AActor* AOwnerTestActor::GetNearestDistance()
{
	// 이 액터로부터 가장 가까운 플레이어 캐릭터를 찾아서 거리 값을 반환한다.
	// 단, maxDistance 까지만 플레이어 검색이 가능하다.
	float minDistance = maxDistance;
	AActor* nearestActor = nullptr;

	// 1. 플레이어들을 모두 검색한다.
#pragma region Blueprint Type
	//TArray<ANetworkProjectCharacter*> players;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANetworkProjectCharacter::StaticClass(), players);
#pragma endregion

#pragma region C++ Type
	for (TActorIterator<ANetworkProjectCharacter> player(GetWorld()); player; ++player)
	{
		ANetworkProjectCharacter* cha = *player;
		// 2. 각 플레이어의 거리 중에서 max Distance 보다 먼 플레이어는 제외한다.
		float curDistance = FVector::Distance(cha->GetActorLocation(), GetActorLocation());

		if(curDistance < minDistance)
		{
			// 3-1. 남은 플레이어 중에서 가장 가까운 플레이어와의 거리 값을 반환한다.
			minDistance = curDistance;
			nearestActor = cha;
		}
	}
#pragma endregion

	//if (minDistance < maxDistance)
	//{
	//	return minDistance;
	//}
	//else
	//{
	//	// 3-2. max Distance 안에 아무도 없으면 maxDistance 값을 반환한다.
	//	return maxDistance;
	//}
	
	if (nearestActor != GetOwner())
	{
		SetOwner(nearestActor);
	}

	return nearestActor;
}


