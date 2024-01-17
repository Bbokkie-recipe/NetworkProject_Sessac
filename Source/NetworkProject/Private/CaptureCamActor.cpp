// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureCamActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"


ACaptureCamActor::ACaptureCamActor()
{
	PrimaryActorTick.bCanEverTick = true;

	camMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cam Mesh"));
	SetRootComponent(camMesh);
	camMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	captureComp = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture Comp"));
	captureComp->SetupAttachment(RootComponent);

}

void ACaptureCamActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACaptureCamActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

