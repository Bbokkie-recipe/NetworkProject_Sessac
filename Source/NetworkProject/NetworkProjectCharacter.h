// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetworkProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetworkProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_releaseWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ia_Fire;

public:
	ANetworkProjectCharacter();
	

protected:

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
			

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class APistolActor* GetOwningWeapon() { return owningWeapon; }
	FORCEINLINE void SetOwningWeapon(class APistolActor* weapon) { owningWeapon = weapon; }
	FORCEINLINE int32 GetAmmo() { return m_Ammo; }
	FORCEINLINE bool GetDeadState() { return bIsDead; }
	
	UPROPERTY(EditAnywhere, Category="MyComponents")
	class UWidgetComponent* playerInfoWidgetComp;

	UPROPERTY(EditAnywhere, Category="MySettings")
	TSubclassOf<class UBattleWidget> battleWidget;

	UPROPERTY(EditAnywhere, Category="MySettings")
	TArray<UAnimMontage*> fireAnimMontage;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	int32 maxHealth = 10;

	UPROPERTY(EditAnywhere, Category = "MySettings")
	TSubclassOf<class UCameraShakeBase> hitShake;

	void SetWeaponInfo(int32 ammo, float damage, float delay);
	void Damaged(int32 dmg);

private:
	enum ENetRole localRole;
	enum ENetRole remoteRole;
	class UBattleWidget* battleUI;
	class UPlayerInfoWidget* info_UI;
	FTimerHandle fireCooltime;
	bool bIsDead = false;
	class APlayerController* pc;

	// 복제 변수
	UPROPERTY(replicated)
	class APistolActor* owningWeapon;

	UPROPERTY(replicated)
	float elapsedTime = 0;

	UPROPERTY(replicated)
	int32 jumpCount = 0;

	UPROPERTY(replicated)
	int32 m_Ammo = 0;

	UPROPERTY(replicated)
	float m_damagePower = 0;

	UPROPERTY(replicated)
	float m_attackDelay = 0;

	UPROPERTY(replicated)
	int32 currentHealth = 0;

	void PrintInfoLog();
	void PrintTimeLog(float DeltaSeconds);
	void JumpStart();
	void ReleaseWeapon();
	void Fire();

	// RPC 함수
	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerJump();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastJump();

	UFUNCTION(Server, Unreliable)
	void ServerFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFire();

	UFUNCTION(Server, Unreliable)
	void ServerDamaged(int32 dmg);

	UFUNCTION(Client, Unreliable)
	void ClientDamaged();

	UFUNCTION(Server, Reliable)
	void ServerDieProcess();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDieProcess();

};

