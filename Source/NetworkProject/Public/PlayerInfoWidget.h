// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKPROJECT_API UPlayerInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="MySettings")
	class UProgressBar* pb_health;

	UPROPERTY(VisibleAnywhere, meta = (BindWidget), Category = "MySettings")
	class UTextBlock* text_name;
};
