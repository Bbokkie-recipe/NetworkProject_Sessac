// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UBattleWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UTextBlock* text_ammo;

	UPROPERTY(VisibleAnywhere, meta = (BindWidgetAnim), Transient, Category = "MySettings")
	class UWidgetAnimation* hitAnim;

	void PlayHitAnimation();

private:
	class ANetworkProjectCharacter* player;

};
