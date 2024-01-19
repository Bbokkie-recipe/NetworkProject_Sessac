// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMActor.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"


ABGMActor::ABGMActor()
{
	PrimaryActorTick.bCanEverTick = false;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(rootComp);

	audioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio Component"));
	audioComp->SetupAttachment(RootComponent);
	audioComp->SetAutoActivate(true);
	//audioComp->set
}

void ABGMActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABGMActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

