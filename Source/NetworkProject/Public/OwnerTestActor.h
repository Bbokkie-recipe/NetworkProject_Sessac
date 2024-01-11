// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OwnerTestActor.generated.h"

UCLASS()
class NETWORKPROJECT_API AOwnerTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AOwnerTestActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnyWhere, Category="MySettings")
	class USceneComponent* rootComp;

	UPROPERTY(EditAnyWhere, Category = "MySettings")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnyWhere, Category = "MySettings")
	float maxDistance = 600;

	

private:
	enum ENetRole localRole;
	enum ENetRole remoteRole;

	void PrintInfoLog();
	class AActor* GetNearestDistance();
	
};
