// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "../NetworkProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	player = Cast<ANetworkProjectCharacter>(GetOwningActor());

	if (player != nullptr)
	{
		playerMovement = player->GetCharacterMovement();
	}
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (player == nullptr || playerMovement == nullptr) return;

	FVector velocity = playerMovement->Velocity;

	// 현재 속력 값 가져오기
	if (velocity.IsNearlyZero())
	{
		speed = 0;
	}
	else
	{
		speed = velocity.Size2D();
	}

	// 현재 방향의 각도를 가져오기
	degree = GetDegreeOfVelocity(velocity, player->GetActorForwardVector(), player->GetActorRightVector());

	// 플레이어의 총 소유 여부 확인
	bHasPistol = player->GetOwningWeapon() != nullptr;
}

float UNetPlayerAnimInstance::GetDegreeOfVelocity(FVector& velocity, FVector forwardVec, FVector rightVec)
{
	float angle = 0;

	// 속도 벡터가 거의 0에 가까우면 각도 계산을 하지 않는다.
	if (!velocity.IsNearlyZero())
	{
		FVector normalVelocity = velocity.GetSafeNormal();

		// 현재 방향 값(각도) 가져오기
		float theta = FMath::Acos(FVector::DotProduct(normalVelocity, forwardVec));
		angle = FMath::RadiansToDegrees(theta);

		float cosT = FVector::DotProduct(normalVelocity, rightVec);
		if (cosT < 0)
		{
			angle *= -1.0f;
		}
	}

	return angle;
}
