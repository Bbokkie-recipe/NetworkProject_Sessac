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
	// �� ���ͷκ��� ���� ����� �÷��̾� ĳ���͸� ã�Ƽ� �Ÿ� ���� ��ȯ�Ѵ�.
	// ��, maxDistance ������ �÷��̾� �˻��� �����ϴ�.
	// 1. �÷��̾���� ��� �˻��Ѵ�.
	// 2. �� �÷��̾��� �Ÿ� �߿��� max Distance ���� �� �÷��̾�� �����Ѵ�.
	// 3-1. ���� �÷��̾� �߿��� ���� ����� �÷��̾���� �Ÿ� ���� ��ȯ�Ѵ�.
	// 3-2. max Distance �ȿ� �ƹ��� ������ maxDistance ���� ��ȯ�Ѵ�.

	
	return 0.0f;
}
