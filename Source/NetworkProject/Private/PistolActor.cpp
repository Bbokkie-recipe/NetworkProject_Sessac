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

	if (player != nullptr && player->GetOwningWeapon() == nullptr && GetOwner() == nullptr)
	{
		GrabPistol(player);
	}
}

void APistolActor::GrabPistol(ANetworkProjectCharacter* player)
{
	// Player Controller�� Possess ���� �ʴ� ���ʹ� Owner�� �־�� RPC�� ����� �� �ִ�.
	if (HasAuthority())
	{
		SetOwner(player);


		// ���� �÷��̾� ��Ʈ�ѷ��� ��쿡, ���� RPC �Լ��� �����Ѵ�.
		//if (player->GetController() && player->GetController()->IsLocalPlayerController())

		ServerGrabPistol_Implementation(player);
	}
}

void APistolActor::ServerGrabPistol_Implementation(ANetworkProjectCharacter* player)
{
	// �÷��̾��� ���� ���� ������ ������ �����Ѵ�.
	player->SetOwningWeapon(this);

	// �� ���� ������ �÷��̾�� �����Ѵ�.
	player->SetWeaponInfo(ammo, damagePower, attackDelay);

	MulticastGrabPistol(player);
}

void APistolActor::MulticastGrabPistol_Implementation(ANetworkProjectCharacter* player)
{
	// ���� �÷��̾� �޽�(����)�� �����Ѵ�.
	boxComp->SetSimulatePhysics(false);
	AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("PistolSocket"));
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// ���� �� Ÿ�Կ� ���� �浹 ������ Ignore�� �����Ѵ�.
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}


void APistolActor::ReleaseWeapon(ANetworkProjectCharacter* player)
{
	// ����, �÷��̾ nullptr�� �ƴ϶��...
	if (player != nullptr)
	{
		ServerReleasePistol(player);
	}
}

void APistolActor::ServerReleasePistol_Implementation(ANetworkProjectCharacter* player)
{
	// �÷��̾��� owningWeapon ������ nullptr�� �����Ѵ�.
	player->SetOwningWeapon(nullptr);

	MulticastReleasePistol(player);

	// PistolActor�� owner�� nullptr�� �����Ѵ�.
	SetOwner(nullptr);
}

void APistolActor::MulticastReleasePistol_Implementation(ANetworkProjectCharacter* player)
{
	// PistolActor�� �÷��̾�κ��� �����.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// boxComp�� �ݸ��� �������� ���� ������ �Ҵ�.
	boxComp->SetSimulatePhysics(true);

	// boxComp�� �ݸ��� ���� ������ ���� �� ���� ȿ���� ����ǰ� �Ѵ�.
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 3�� �ڿ� ���� �� Ÿ�Կ� ���� �浹 ������ Overlap�� �����Ѵ�.
	FTimerHandle collisionHandle;
	GetWorldTimerManager().SetTimer(collisionHandle, this, &APistolActor::SetCollisionResponse, 3.0f, false);
}

void APistolActor::SetCollisionResponse()
{
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
