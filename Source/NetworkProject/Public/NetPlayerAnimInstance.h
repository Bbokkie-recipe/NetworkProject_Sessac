// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MySettings")
	bool bStartAnim = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MySettings")
	bool bHasPistol = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MySettings")
	float degree = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MySettings")
	float speed = 0;

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	class ANetworkProjectCharacter* player;
	class UCharacterMovementComponent* playerMovement;

	float GetDegreeOfVelocity(FVector& velocity, FVector forwardVec, FVector rightVec);
};
