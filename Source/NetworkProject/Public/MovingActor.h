// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingActor.generated.h"

UCLASS()
class NETWORKPROJECT_API AMovingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMovingActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UStaticMeshComponent* meshComp;

	FORCEINLINE int32 GetBoardingCount() { return boardingCount; }
	FORCEINLINE void SetBoardingCount(int32 num) { boardingCount += num; }
	FORCEINLINE class ATestCharacter* GetPushMan() { return pushMan; }
	FORCEINLINE void SetPushMan(class ATestCharacter* man) { pushMan =  man; }

private:
	int32 boardingCount = 0;
	class ATestCharacter* pushMan;
};
