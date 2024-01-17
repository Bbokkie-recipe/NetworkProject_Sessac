// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureCamActor.generated.h"

UCLASS()
class NETWORKPROJECT_API ACaptureCamActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureCamActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category="MySettings")
	class UStaticMeshComponent* camMesh;

	UPROPERTY(VisibleAnywhere, Category = "MySettings")
	class USceneCaptureComponent2D* captureComp;
};
