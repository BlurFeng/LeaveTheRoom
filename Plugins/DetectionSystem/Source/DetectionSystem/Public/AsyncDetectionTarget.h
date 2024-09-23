// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncDetectionTarget.generated.h"

/**
 *
 */
UCLASS()
class DETECTIONSYSTEM_API UAsyncDetectionTarget : public UBlueprintAsyncActionBase
{
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDetectionTargetDelegate, bool, haveTarget, const TArray<AActor*>&, TargetActors);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDetectionTargetDelegateSimple);
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
		FDetectionTargetDelegate Complete;

	UPROPERTY(BlueprintAssignable)
		FDetectionTargetDelegateSimple Failed;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", DisplayName = "DetectionTarget", ScriptName = "DetectionTargetDelegate", AdvancedDisplay = "IgnoreActors"), Category = "DetectionSystem|Async")
		static UAsyncDetectionTarget* UAsync_DetectionTarget(UObject* WorldContextObject, UObject* Querier, FDetectionCondition DetectionCondition, TArray<AActor*> IgnoreActors);

private:
	UFUNCTION()
		void CallBackOnEQSFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UPROPERTY()
		const UObject* WorldContextObject;

	UPROPERTY()
		FDetectionCondition DetectionCondition;

	UPROPERTY()
		TArray<AActor*> TargetActors;

	UPROPERTY()
		TArray<AActor*> IgnoreActors;
};