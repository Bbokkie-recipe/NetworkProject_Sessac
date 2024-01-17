// Fill out your copyright notice in the Description page of Project Settings.


#include "VacuumActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DustActor.h"

AVacuumActor::AVacuumActor()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	SetRootComponent(boxComp);
	boxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mouse = CreateDefaultSubobject<USceneComponent>(TEXT("mouse"));
	mouse->SetupAttachment(meshComp);

}

void AVacuumActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVacuumActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<FOverlapResult> hitInfos;
	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel2);

	if (GetWorld()->OverlapMultiByObjectType(hitInfos, mouse->GetComponentLocation(), FQuat::Identity, objParams, FCollisionShape::MakeSphere(50)))
	{
		for (const FOverlapResult& hitObj : hitInfos)
		{
			ADustActor* dust = Cast<ADustActor>(hitObj.GetActor());
			if (dust != nullptr)
			{
				dust->Cleaning(DeltaTime, mouse->GetComponentLocation());
			}
		}
	}
}

