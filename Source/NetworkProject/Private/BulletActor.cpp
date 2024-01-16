// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "../NetworkProjectCharacter.h"


ABulletActor::ABulletActor()
{
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(50.0f);
	sphereComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	sphereComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	sphereComp->SetWorldScale3D(FVector(0.2f));
	sphereComp->SetSimulatePhysics(false);
	sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(5.0f);

	bReplicates = true;
	SetReplicateMovement(true);
}

void ABulletActor::BeginPlay()
{
	Super::BeginPlay();

	sphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABulletActor::OnOverlap);

}

void ABulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		SetActorLocation(GetActorLocation() + GetActorForwardVector() * moveSpeed * DeltaTime);
	}
}

void ABulletActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit!"));
	if (HasAuthority())
	{
		if (ANetworkProjectCharacter* player = Cast<ANetworkProjectCharacter>(OtherActor))
		{
			player->Damaged(damage);
		}
		Destroy();
	}
}

