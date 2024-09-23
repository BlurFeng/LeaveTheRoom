// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncDetectionTarget.h"

#include <DetectionSystem/Public/DetectionSystemUtility.h>
#include "EnvironmentQuery/EnvQueryManager.h"

UAsyncDetectionTarget* UAsyncDetectionTarget::UAsync_DetectionTarget(UObject* WorldContextObject, UObject* Querier, FDetectionCondition DetectionCondition, TArray<AActor*> IgnoreActors)
{
	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UAsyncDetectionTarget* asyncDetectionTarget = NewObject<UAsyncDetectionTarget>();
	asyncDetectionTarget->WorldContextObject = WorldContextObject;
	if (world == nullptr)
	{
		FFrame::KismetExecutionMessage(TEXT("Invalid WorldContextObject. Cannot execute Set Timer."), ELogVerbosity::Error);
		asyncDetectionTarget->Failed.Broadcast();
		asyncDetectionTarget->SetReadyToDestroy();
		return nullptr;
	}

	UEnvQueryInstanceBlueprintWrapper* envQueryInstance;
	if (!UDetectionSystemUtility::DetectionTarget(WorldContextObject, Querier, DetectionCondition, IgnoreActors, envQueryInstance) || envQueryInstance == nullptr)
	{
		asyncDetectionTarget->Failed.Broadcast();
		asyncDetectionTarget->SetReadyToDestroy();
		return nullptr;
	}

	asyncDetectionTarget->DetectionCondition = DetectionCondition;
	asyncDetectionTarget->IgnoreActors = IgnoreActors;

	if (envQueryInstance != nullptr && asyncDetectionTarget != nullptr)
	{
		envQueryInstance->GetOnQueryFinishedEvent().AddDynamic(asyncDetectionTarget, &UAsyncDetectionTarget::CallBackOnEQSFinished);
	}

	return asyncDetectionTarget;
}

void UAsyncDetectionTarget::CallBackOnEQSFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	//获取所有探测到的目标
	TArray<AActor*> InRangeActors;
	QueryInstance->GetQueryResultsAsActors(InRangeActors);

	//获取指定数量的目标
	TargetActors.Empty();
	if (InRangeActors.Num() > 0)
	{
		for (int i = 0; i < DetectionCondition.Amount; i++)
		{
			if (!InRangeActors.IsValidIndex(i) || IgnoreActors.Contains(InRangeActors[i]))
				break;

			TargetActors.Add(InRangeActors[i]);
		}
	}

	Complete.Broadcast(TargetActors.Num() > 0, TargetActors);
	SetReadyToDestroy();
}