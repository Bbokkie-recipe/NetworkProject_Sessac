// Fill out your copyright notice in the Description page of Project Settings.


#include "OwnerTestActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"


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

	PrintInfoLog();

	float dist = GetNearestDistance();
	UE_LOG(LogTemp, Warning, TEXT("Nearest is %.2f"), dist);
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

float AOwnerTestActor::GetNearestDistance()
{
	// 이 액터로부터 가장 가까운 플레이어 캐릭터를 찾아서 거리 값을 반환한다.
	// 단, maxDistance 까지만 플레이어 검색이 가능하다.
	// 1. 플레이어들을 모두 검색한다.
	// 2. 각 플레이어의 거리 중에서 max Distance 보다 먼 플레이어는 제외한다.
	// 3-1. 남은 플레이어 중에서 가장 가까운 플레이어와의 거리 값을 반환한다.
	// 3-2. max Distance 안에 아무도 없으면 maxDistance 값을 반환한다.

	
	return 0.0f;
}
