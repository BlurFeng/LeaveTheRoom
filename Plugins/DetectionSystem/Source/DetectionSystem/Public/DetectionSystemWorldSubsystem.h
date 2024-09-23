// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DetectionSystemWorldSubsystem.generated.h"

class ADetectionSystemDebugHUD;

USTRUCT()
struct FOcclusionExamineTraceInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
		bool IsUpdate;

	UPROPERTY()
		bool IsNeedReset;

	UPROPERTY()
		bool IsCanRecycle;

	/*射线结束点位置*/
	UPROPERTY()
		FVector EndLocation;

	/*是否是成功Hit到目标Item的射线*/
	UPROPERTY()
		bool SucceedHit;

	UPROPERTY()
		FVector2D ForecastTraceScreenPos;

	FOcclusionExamineTraceInfo() {}

	FOcclusionExamineTraceInfo(FVector InEndLocation, bool InSucceedHit, FVector2D InForecastTraceScreenPos)
		: EndLocation(InEndLocation)
		, SucceedHit(InSucceedHit)
		, ForecastTraceScreenPos(InForecastTraceScreenPos)
	{
	}
};

/**
 * 
 */
UCLASS()
class DETECTIONSYSTEM_API UDetectionSystemWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

public:

	UFUNCTION()
		ADetectionSystemDebugHUD* GetDetectionSystemDebugHUD();
	
private:

	UPROPERTY()
		ADetectionSystemDebugHUD* DetectionSystemDebugHUD;

#pragma region UEnvQueryTest_ScreenRange

public:

	UFUNCTION()
		void UpdateForecastTracePos(AActor* ActorKey, FOcclusionExamineTraceInfo OcclusionExamineTraceInfo);

	UFUNCTION()
		bool GetForecastTracePos(AActor* ActorKey, FOcclusionExamineTraceInfo& OcclusionExamineTraceInfo);

	UFUNCTION()
		void OnUEnvQueryTestScreenRangeOver();

private:

	UPROPERTY()
		TMap<AActor*, FOcclusionExamineTraceInfo> UEQ_ScreenRange_ForecastTraceInfoMap;

	UPROPERTY()
		int RefreshOcclusionExamineTraceInfoRecycleCounter;

#pragma endregion
};
