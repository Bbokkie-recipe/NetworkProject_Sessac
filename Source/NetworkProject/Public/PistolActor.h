// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PistolActor.generated.h"

UCLASS()
class NETWORKPROJECT_API APistolActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APistolActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="MySettings")
	class UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	TSubclassOf<class ABulletActor> bullet_bp;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	int32 ammo = 10;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	float damagePower = 2.0f;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	float attackDelay = 1.0f;

	void ReleaseWeapon(class ANetworkProjectCharacter* player);
	void Fire(class ANetworkProjectCharacter* player);

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void GrabPistol(class ANetworkProjectCharacter* player);
	void SetCollisionResponse();

	UFUNCTION(Server, Unreliable)
	void ServerGrabPistol(class ANetworkProjectCharacter* player);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastGrabPistol(class ANetworkProjectCharacter* player);

	UFUNCTION(Server, Unreliable)
	void ServerReleasePistol(class ANetworkProjectCharacter* player);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastReleasePistol(class ANetworkProjectCharacter* player);
};
