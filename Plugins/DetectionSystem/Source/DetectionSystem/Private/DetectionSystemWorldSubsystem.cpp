// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionSystemWorldSubsystem.h"

#include "DetectionSystem/Widget/DetectionSystemDebugHUD.h"

#include "Debug/DebugDrawService.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UDetectionSystemWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{

}

void UDetectionSystemWorldSubsystem::Deinitialize()
{
	if (DetectionSystemDebugHUD != nullptr)
	{
		DetectionSystemDebugHUD->Destroy();
		DetectionSystemDebugHUD = nullptr;
	}

	UEQ_ScreenRange_ForecastTraceInfoMap.Empty();
}

ADetectionSystemDebugHUD* UDetectionSystemWorldSubsystem::GetDetectionSystemDebugHUD()
{
	if (DetectionSystemDebugHUD == nullptr || !UKismetSystemLibrary::IsValid(DetectionSystemDebugHUD))
	{
		UWorld* world = GetWorld();
		if (world && GetWorld()->HasBegunPlay())
		{
			static FDelegateHandle DrawDebugDelegateHandle;
			TArray<AActor*> FindDetectionSystemDebugHUDs;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADetectionSystemDebugHUD::StaticClass(), FindDetectionSystemDebugHUDs);
			if (FindDetectionSystemDebugHUDs.IsEmpty())
			{
				DetectionSystemDebugHUD = GetWorld()->SpawnActor<ADetectionSystemDebugHUD>();

				FDebugDrawDelegate DrawDebugDelegate = FDebugDrawDelegate::CreateUObject(DetectionSystemDebugHUD, &ADetectionSystemDebugHUD::DrawDebugHUD);
				DrawDebugDelegateHandle = UDebugDrawService::Register(TEXT("GameplayDebug"), DrawDebugDelegate);
			}
			else
			{
				DetectionSystemDebugHUD = Cast<ADetectionSystemDebugHUD>(FindDetectionSystemDebugHUDs[0]);
			}
		}
	}

	return DetectionSystemDebugHUD;
}

void UDetectionSystemWorldSubsystem::UpdateForecastTracePos(AActor* ActorKey, FOcclusionExamineTraceInfo OcclusionExamineTraceInfo)
{
	OcclusionExamineTraceInfo.IsUpdate = true;
	OcclusionExamineTraceInfo.IsCanRecycle = false;

	if (!UEQ_ScreenRange_ForecastTraceInfoMap.Contains(ActorKey))
	{
		UEQ_ScreenRange_ForecastTraceInfoMap.Add(ActorKey, OcclusionExamineTraceInfo);
	}
	else
	{
		UEQ_ScreenRange_ForecastTraceInfoMap[ActorKey] = OcclusionExamineTraceInfo;
	}
}

bool UDetectionSystemWorldSubsystem::GetForecastTracePos(AActor* ActorKey, FOcclusionExamineTraceInfo& OcclusionExamineTraceInfo)
{
	if (UEQ_ScreenRange_ForecastTraceInfoMap.Contains(ActorKey))
	{
		OcclusionExamineTraceInfo = UEQ_ScreenRange_ForecastTraceInfoMap[ActorKey];
		bool isNeedReset = OcclusionExamineTraceInfo.IsNeedReset;
		if (isNeedReset)OcclusionExamineTraceInfo.IsNeedReset = false;
		return !isNeedReset;
	}

	OcclusionExamineTraceInfo = FOcclusionExamineTraceInfo();
	return false;
}

void UDetectionSystemWorldSubsystem::OnUEnvQueryTestScreenRangeOver()
{
	if (!UEQ_ScreenRange_ForecastTraceInfoMap.IsEmpty())
	{
		//每次更新
		for (auto& item : UEQ_ScreenRange_ForecastTraceInfoMap)
		{
			if (!item.Value.IsUpdate)
			{
				//每次停止更新后，下次再开始更新需要重置数据
				item.Value.IsNeedReset = true;
			}

			item.Value.IsUpdate = false;
		}

		//间隔一段时间刷新，确认ForecastTraceInfoMap中是否有信息长时间未更新
		//未更新的我们将从Map中移除
		RefreshOcclusionExamineTraceInfoRecycleCounter++;
		if (RefreshOcclusionExamineTraceInfoRecycleCounter >= 200)
		{
			RefreshOcclusionExamineTraceInfoRecycleCounter = 0;

			TArray<AActor*> recycleItemKeys;
			for (auto& item : UEQ_ScreenRange_ForecastTraceInfoMap)
			{
				if (!item.Value.IsCanRecycle)
				{
					item.Value.IsCanRecycle = true;
				}
				else
				{
					recycleItemKeys.Add(item.Key);
				}
			}

			if (!recycleItemKeys.IsEmpty())
			{
				for (int i = 0; i < recycleItemKeys.Num(); i++)
				{
					UEQ_ScreenRange_ForecastTraceInfoMap.Remove(recycleItemKeys[i]);
				}

				recycleItemKeys.Empty();
			}
		}
	}
}