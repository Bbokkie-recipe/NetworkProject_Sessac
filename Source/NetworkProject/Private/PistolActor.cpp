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
	// Player Controller가 Possess 되지 않는 액터는 Owner가 있어야 RPC를 사용할 수 있다.
	if (HasAuthority())
	{
		SetOwner(player);


		// 로컬 플레이어 컨트롤러일 경우에, 서버 RPC 함수를 실행한다.
		//if (player->GetController() && player->GetController()->IsLocalPlayerController())

		ServerGrabPistol_Implementation(player);
	}
}

void APistolActor::ServerGrabPistol_Implementation(ANetworkProjectCharacter* player)
{
	// 플레이어의 소유 무기 변수를 총으로 설정한다.
	player->SetOwningWeapon(this);

	// 이 총의 정보를 플레이어에게 전달한다.
	player->SetWeaponInfo(ammo, damagePower, attackDelay);

	MulticastGrabPistol(player);
}

void APistolActor::MulticastGrabPistol_Implementation(ANetworkProjectCharacter* player)
{
	// 총을 플레이어 메시(소켓)에 부착한다.
	boxComp->SetSimulatePhysics(false);
	AttachToComponent(player->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("PistolSocket"));
	boxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// 총의 폰 타입에 대한 충돌 응답을 Ignore로 변경한다.
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}


void APistolActor::ReleaseWeapon(ANetworkProjectCharacter* player)
{
	// 만일, 플레이어가 nullptr가 아니라면...
	if (player != nullptr)
	{
		ServerReleasePistol(player);
	}
}

void APistolActor::ServerReleasePistol_Implementation(ANetworkProjectCharacter* player)
{
	// 플레이어의 owningWeapon 변수를 nullptr로 지정한다.
	player->SetOwningWeapon(nullptr);

	MulticastReleasePistol(player);

	// PistolActor의 owner를 nullptr로 지정한다.
	SetOwner(nullptr);
}

void APistolActor::MulticastReleasePistol_Implementation(ANetworkProjectCharacter* player)
{
	// PistolActor를 플레이어로부터 떼어낸다.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// boxComp의 콜리젼 설정에서 물리 설정을 켠다.
	boxComp->SetSimulatePhysics(true);

	// boxComp의 콜리전 응답 설정을 쿼리 및 물리 효과에 적용되게 한다.
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// 3초 뒤에 총의 폰 타입에 대한 충돌 응답을 Overlap로 변경한다.
	FTimerHandle collisionHandle;
	GetWorldTimerManager().SetTimer(collisionHandle, this, &APistolActor::SetCollisionResponse, 3.0f, false);
}

void APistolActor::SetCollisionResponse()
{
	boxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
