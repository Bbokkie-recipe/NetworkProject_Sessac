// Fill out your copyright notice in the Description page of Project Settings.


#include "DustActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

ADustActor::ADustActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADustActor::BeginPlay()
{
	Super::BeginPlay();

	originScale = GetActorScale();
	originLoc = GetActorLocation();
}

void ADustActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	time += DeltaTime * 5;
	FVector loc = originLoc + FVector(0, 0, FMath::Sin(time) * 50);
	SetActorLocation(loc);
}

void ADustActor::Cleaning(float deltaTime, FVector targetLoc)
{
	process += deltaTime * 0.3f;

	if (process < 1.0f)
	{
		SetActorScale3D(FMath::Lerp(originScale, originScale * 0.5f, process));
		SetActorLocation(FMath::Lerp(originLoc, targetLoc, process));
	}
}

