// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "DetectionSystemDebugHUD.generated.h"

USTRUCT()
struct DETECTIONSYSTEM_API FDetectionSystemDebug2DBoxInfo
{
	GENERATED_USTRUCT_BODY()

		FDetectionSystemDebug2DBoxInfo() { }
	FDetectionSystemDebug2DBoxInfo(FVector2D NewPosition, FLinearColor NewColor, float NewSize = 4.f)
	{
		Position = NewPosition;
		Color = NewColor;
		Size = NewSize;
	}

public:

	UPROPERTY()
		FVector2D Position;

	UPROPERTY()
		FLinearColor Color;

	UPROPERTY()
		float Size;
};

USTRUCT()
struct DETECTIONSYSTEM_API FDetectionSystemDebugTextInfo
{
	GENERATED_USTRUCT_BODY()

		FDetectionSystemDebugTextInfo() { }
	FDetectionSystemDebugTextInfo(FString NewString, FVector NewLocation, FColor NewColor)
	{
		String = NewString;
		Location = NewLocation;
		Color = NewColor;
	}

public:

	UPROPERTY()
		FString String;

	UPROPERTY()
		FVector Location;

	UPROPERTY()
		FColor Color;
};

/**
 * 
 */
UCLASS()
class DETECTIONSYSTEM_API ADetectionSystemDebugHUD : public AHUD
{
	GENERATED_BODY()

	ADetectionSystemDebugHUD();
public:

	virtual void Tick(float DeltaSeconds) override;

	void DrawDebugHUD(UCanvas* Canvas, APlayerController* PC);

	UFUNCTION()
		void OnEndDraw();
	
	UFUNCTION()
		UCanvas* GetCanvas();

#pragma region EQTScreenDistance

	UPROPERTY()
		bool bDrawDrawEQTScreenDistanceRange;

	UPROPERTY()
		FVector2D DrawEQTScreenDistanceRange_Center;

	UPROPERTY()
		float DrawEQTScreenDistanceRange_Radius;

	UPROPERTY()
		TArray<FDetectionSystemDebug2DBoxInfo> DrawEQTScreenRange_ItemPoints;

	UPROPERTY()
		TArray<FDetectionSystemDebugTextInfo> DrawEQTScreenRange_Score;

	UFUNCTION()
		void SetDrawEQTScreenRange(const FVector2D& Center, float Radius);

	UFUNCTION()
		void DisableDrawEQTScreenRange();

	UFUNCTION()
		void SetDrawEQTScreenRange_ItemPoint(const FVector2D& Point, FLinearColor Color);

	UFUNCTION()
		void SetDrawEQTScreenRange_Clear();

	UFUNCTION()
		void SetDrawEQTScreenRange_Score(const FString& String, const FVector& Location, FColor Color);

#pragma endregion
};
