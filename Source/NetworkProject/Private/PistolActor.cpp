// Fill out your copyright notice in the Description page of Project Settings.


#include "PistolActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "../NetworkProjectCharacter.h"


APistolActor::APistolActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxComp->SetGenerateOverlapEvents(true);
	boxComp->SetSimulatePhysics(true);
	boxComp->SetEnableGravity(true);
	boxComp->SetCollisionObjectType(ECC_GameTraceChannel1);
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	boxComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;
	SetReplicateMovement(true);
}

void APistolActor::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &APistolActor::OnOverlap);
}

void APistolActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APistolActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANetworkProjectCharacter* player = Cast<ANetworkProjectCharacter>(OtherActor);

	if (player != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning Weapon: %s"), player->GetOwningWeapon() == nullptr ? *FString("Null") : *FString("Have Weapon"));
	}

	if (player != nullptr && player->GetOwningWeapon() == nullptr && GetOwner() == nullptr)
	{
		// 1. 총을 플레이어 메시(소켓)에 부착한다.
		boxComp->SetSimulatePhysics(false);
		AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("PistolSocket"));
		boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 2. 총의 오너를 플레이어로 설정하고, 플레이어의 소유 무기 변수를 총으로 설정한다.
		if (HasAuthority())
		{
			SetOwner(player);
			player->SetOwningWeapon(this);
		}
	}
}

