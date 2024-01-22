// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "PistolActor.h"
#include "BattleWidget.h"
#include "Components/WidgetComponent.h"
#include "PlayerInfoWidget.h"
#include "Components/ProgressBar.h"
#include "NetworkGameInstance.h"
#include "NetPlayerState.h"
#include "Components/TextBlock.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);


ANetworkProjectCharacter::ANetworkProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	playerInfoWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("Info Widget"));
	playerInfoWidgetComp->SetupAttachment(GetMesh());
	playerInfoWidgetComp->SetDrawSize(FVector2D(200));

	bReplicates = true;
	SetReplicateMovement(true);
}

void ANetworkProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	pc = GetController<APlayerController>();
	gi = GetGameInstance<UNetworkGameInstance>();

	if (pc != nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	localRole = GetLocalRole();
	remoteRole = GetRemoteRole();
	info_UI = Cast<UPlayerInfoWidget>(playerInfoWidgetComp->GetWidget());

	if (battleWidget != nullptr && GetController() && GetController()->IsLocalPlayerController())
	{
		battleUI = CreateWidget<UBattleWidget>(GetWorld(), battleWidget);

		if (battleUI != nullptr)
		{
			battleUI->AddToViewport();
		}
	}

	if (HasAuthority())
	{
		currentHealth = maxHealth;
	}

	if (gi != nullptr && GetController() && GetController()->IsLocalPlayerController())
	{
		//FColor fakeColor = FColor(0, 255, 0);
		//ChangeMeshAndColor(gi->meshNum, gi->meshColor);
		ServerSetMeshAndColor(gi->meshNum, gi->meshColor);
	}

	FTimerHandle ChangeHandler;
	GetWorldTimerManager().SetTimer(ChangeHandler, this, &ANetworkProjectCharacter::ChangeMeshAndColor, 0.5f, false);
}

void ANetworkProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PrintInfoLog();
	//PrintTimeLog(DeltaSeconds);

	if (info_UI != nullptr)
	{
		info_UI->pb_health->SetPercent((float)currentHealth / (float)maxHealth);
	}

	// 죽음 상태 체크
	if (!bIsDead && currentHealth <= 0)
	{
		bIsDead = true;
		ServerDieProcess();
	}
}

void ANetworkProjectCharacter::SetWeaponInfo(int32 ammo, float damage, float delay)
{
	m_Ammo = ammo;
	m_damagePower = damage;
	m_attackDelay = delay;
}

#pragma region DamageRPC

void ANetworkProjectCharacter::Damaged(int32 dmg)
{
	ServerDamaged(dmg);
}

void ANetworkProjectCharacter::ServerDamaged_Implementation(int32 dmg)
{
	// 현재 체력 상태 데미지 처리
	currentHealth = FMath::Max(0, currentHealth - dmg);

	// 해당 클라이언트(로컬)에서 실행할 내용 호출
	ClientDamaged();
}

void ANetworkProjectCharacter::ClientDamaged_Implementation()
{
	// 피격 효과 UI 애니메니션 플레이
	battleUI->PlayHitAnimation();

	// 카메라 쉐이크 효과 주기
	if (pc != nullptr && hitShake != nullptr)
	{
		pc->ClientStartCameraShake(hitShake);
	}
}
#pragma endregion

#pragma region Status Log

void ANetworkProjectCharacter::PrintInfoLog()
{
	// 1. 네트워크 롤 확인용 로그
	//FString characterName = GetActorNameOrLabel();
	//FString localRoleString = UEnum::GetValueAsString<ENetRole>(localRole);
	//FString remoteRoleString = UEnum::GetValueAsString<ENetRole>(remoteRole);
	//FString ownerString = GetOwner() == nullptr ? *FString("No Owner") : *GetOwner()->GetActorNameOrLabel();
	//FString connectionString = GetNetConnection() == nullptr ? *FString("Invalid Connection") : *FString("Valid Connection");

	//FString printString = FString::Printf(TEXT("Player Name: %s\nLocal Role: %s\nRemote Role: %s\nOwner: %s\nNet Connection: %s\n"), *characterName, *localRoleString, *remoteRoleString, *ownerString, *connectionString);

	// 2. 게임프레임워크 확인용 로그
	FString gameModeString = GetWorld()->GetAuthGameMode() != nullptr ? *FString("Valid") : *FString("Invalid");
	FString gameStateString = GetWorld()->GetGameState() != nullptr ? *FString("Valid") : *FString("Invalid");
	FString playerStateString = GetPlayerState() != nullptr ? *FString("Valid") : *FString("Invalid");
	AHUD* hud = GetController<APlayerController>() != nullptr ? GetController<APlayerController>()->GetHUD() : nullptr;
	FString HUDString = hud != nullptr ? *FString("Valid") : *FString("Invalid");

	FString printString = FString::Printf(TEXT("GameMode: %s\nGameState: %s\nPlayerSate: %s\nHUD: %s"), *gameModeString, *gameStateString, *playerStateString, *HUDString);

	DrawDebugString(GetWorld(), GetActorLocation(), printString, nullptr, FColor::White, 0, true, 1.0f);
}

void ANetworkProjectCharacter::PrintTimeLog(float DeltaSeconds)
{
	//if (GetLocalRole() == ENetRole::ROLE_Authority)
	if (HasAuthority())
	{
		elapsedTime += DeltaSeconds;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Elapsed Time: %.2f"), elapsedTime);
	DrawDebugString(GetWorld(), GetActorLocation(), FString::Printf(TEXT("Elapsed Time: %.2f\nJump Count: %d"), elapsedTime, jumpCount), nullptr, FColor::White, 0, true, 1.0f);
}
#pragma endregion

#pragma region InputSetup

void ANetworkProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANetworkProjectCharacter::JumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetworkProjectCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetworkProjectCharacter::Look);
		EnhancedInputComponent->BindAction(ia_releaseWeapon, ETriggerEvent::Started, this, &ANetworkProjectCharacter::ReleaseWeapon);
		EnhancedInputComponent->BindAction(ia_Fire, ETriggerEvent::Started, this, &ANetworkProjectCharacter::Fire);
		EnhancedInputComponent->BindAction(ia_VoiceControl, ETriggerEvent::Started, this, &ANetworkProjectCharacter::VoiceChatOn);
		EnhancedInputComponent->BindAction(ia_VoiceControl, ETriggerEvent::Completed, this, &ANetworkProjectCharacter::VoiceChatOff);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetworkProjectCharacter::Move(const FInputActionValue& Value)
{
	//if(bIsDead) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetworkProjectCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
#pragma endregion

#pragma region JumpRPC

void ANetworkProjectCharacter::JumpStart()
{
	if (HasAuthority())
	{
		ServerJump_Implementation();
	}
	else
	{
		ServerJump();
	}
}

// 서버에서만 실행할 내용
void ANetworkProjectCharacter::ServerJump_Implementation()
{
	jumpCount++;
	repJumpCount++;
	MulticastJump();

	//UE_LOG(LogTemp, Warning, TEXT("SeverJump Called!!!"))
}

// 서버에 요청 시 유효한 요청인지를 검증
bool ANetworkProjectCharacter::ServerJump_Validate()
{
	return true;
}

// 모든 클라이언트에서 동시에 실행할 내용
void ANetworkProjectCharacter::MulticastJump_Implementation()
{
	Jump();
	//UE_LOG(LogTemp, Warning, TEXT("MulticastJump Called!!!"))
}
#pragma endregion

#pragma region ReleaseWeapon

void ANetworkProjectCharacter::ReleaseWeapon()
{
	//UE_LOG(LogTemp, Warning, TEXT("%s(%d): Release Weapon!"), *FString(__FUNCTION__), __LINE__);

	if (owningWeapon != nullptr)
	{
		owningWeapon->ReleaseWeapon(this);
	}
}

#pragma endregion

#pragma region VoiceChat

void ANetworkProjectCharacter::VoiceChatOn()
{
	pc->StartTalking();
}

void ANetworkProjectCharacter::VoiceChatOff()
{
	pc->StopTalking();
	//pc->GetUniqueID()
	//pc->ServerMutePlayer(0);
}

#pragma endregion

#pragma region FireRPC

void ANetworkProjectCharacter::Fire()
{
	if (owningWeapon != nullptr)
	{
		if (!GetWorldTimerManager().IsTimerActive(fireCooltime))
		{
			ServerFire();
		}
	}
}

void ANetworkProjectCharacter::ServerFire_Implementation()
{
	if (m_Ammo > 0)
	{
		owningWeapon->Fire(this);
		m_Ammo = FMath::Max(0, m_Ammo - 1);
	}
	MulticastFire();
}

void ANetworkProjectCharacter::MulticastFire_Implementation()
{
	bool bHasAmmo = m_Ammo > 0;
	PlayAnimMontage(fireAnimMontage[(int32)bHasAmmo]);
	GetWorldTimerManager().SetTimer(fireCooltime, m_attackDelay, false);
}

#pragma endregion

#pragma region Die Process RPC

void ANetworkProjectCharacter::ServerDieProcess_Implementation()
{
	MulticastDieProcess();
}

void ANetworkProjectCharacter::MulticastDieProcess_Implementation()
{
	// 입력 방지
	GetCharacterMovement()->DisableMovement();
	bUseControllerRotationYaw = false;
	CameraBoom->bUsePawnControlRotation = false;

	// 총을 내려놓는다.
	ReleaseWeapon();

	// 충돌 처리 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (pc != nullptr && pc->IsLocalPlayerController())
	{
		FTimerHandle dieEffectHandler;
		GetWorldTimerManager().SetTimer(dieEffectHandler, FTimerDelegate::CreateLambda([&]() {

			// 세션 나가기 버튼을 화면에 표시한다.
			battleUI->ShowButtons();
			pc->SetShowMouseCursor(true);
			pc->SetInputMode(FInputModeUIOnly());

			// 화면을 흑백으로 후 처리한다.
			FollowCamera->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 0.5f);
			}), 1.1f, false);

	}
}
#pragma endregion

// 변수 복제로 값이 변경될 때마다 실행될 함수
void ANetworkProjectCharacter::OnRep_JumpEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("Replicated Call Function!!"));
	if (battleUI != nullptr)
	{
		battleUI->PlayHitAnimation();
	}
}


void ANetworkProjectCharacter::ChangeMeshAndColor()
{
	// runtime에 특정 경로에 있는 에셋을 메모리에 로드하기(인스턴스화)
	USkeletalMesh* selectedMesh = LoadObject<USkeletalMesh>(NULL, *playerMeshes[playerMeshNum], NULL, LOAD_None, NULL);
	if (selectedMesh != nullptr)
	{
		// 현재 메시에 로드한 메시를 설정한다.
		GetMesh()->SetSkeletalMesh(selectedMesh);
	}

	// 기존 머티리얼을 가져오기
	UMaterialInterface* mat_0 = GetMesh()->GetMaterial(0);
	UMaterialInterface* mat_1 = GetMesh()->GetMaterial(1);

	// 다이나믹 인스턴스 머티리얼로 변환하기
	UMaterialInstanceDynamic* mat_inst_0 = UMaterialInstanceDynamic::Create(mat_0, this);
	UMaterialInstanceDynamic* mat_inst_1 = UMaterialInstanceDynamic::Create(mat_1, this);

	// 머티리얼의 벡터 변수 값을 매개변수의 컬러 값으로 변경한다.
	mat_inst_0->SetVectorParameterValue(FName("playerColor"), (FLinearColor)playerColor);
	mat_inst_1->SetVectorParameterValue(FName("playerColor"), (FLinearColor)playerColor);

	GetMesh()->SetMaterial(0, mat_inst_0);
	GetMesh()->SetMaterial(1, mat_inst_1);

	if (info_UI != nullptr)
	{
		FString myName = GetPlayerState<ANetPlayerState>()->GetPlayerName();
		info_UI->text_name->SetText(FText::FromString(myName));

		if (GetController() && GetController()->IsLocalPlayerController())
		{
			info_UI->text_name->SetColorAndOpacity(FSlateColor(FColor::Magenta));
		}
	}
}

void ANetworkProjectCharacter::ServerSetMeshAndColor_Implementation(int32 meshNum, FColor meshColor)
{
	playerMeshNum = meshNum;
	playerColor = meshColor;
}


void ANetworkProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ANetworkProjectCharacter, elapsedTime);
	DOREPLIFETIME(ANetworkProjectCharacter, jumpCount);
	DOREPLIFETIME_CONDITION(ANetworkProjectCharacter, elapsedTime, COND_AutonomousOnly);
	DOREPLIFETIME(ANetworkProjectCharacter, owningWeapon);
	DOREPLIFETIME(ANetworkProjectCharacter, m_Ammo);
	DOREPLIFETIME(ANetworkProjectCharacter, m_damagePower);
	DOREPLIFETIME(ANetworkProjectCharacter, m_attackDelay);
	DOREPLIFETIME(ANetworkProjectCharacter, currentHealth);
	DOREPLIFETIME(ANetworkProjectCharacter, repJumpCount);
	DOREPLIFETIME(ANetworkProjectCharacter, playerMeshNum);
	DOREPLIFETIME(ANetworkProjectCharacter, playerColor);
}


