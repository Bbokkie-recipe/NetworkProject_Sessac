// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DustActor.generated.h"

UCLASS()
class NETWORKPROJECT_API ADustActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ADustActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UStaticMeshComponent* meshComp;

	void Cleaning(float deltaTime, FVector targetLoc);

private:
	float process = 0;
	float time = 0;

	FVector originScale;
	FVector originLoc;
};
