// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

AMovingActor::AMovingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMovingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMovingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (boardingCount >= 3)
	{
		FVector getVelo = pushMan->GetCharacterMovement()->Velocity;
		getVelo.Z = 0;
		SetActorLocation(getVelo, true);
	}
}

