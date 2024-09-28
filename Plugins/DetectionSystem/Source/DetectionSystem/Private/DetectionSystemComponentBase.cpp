// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionSystemComponentBase.h"

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>
#include "DetectionObjectInterface.h"
#include "DetectionSystemUtility.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "EnvironmentQuery/EnvQueryManager.h"

// Sets default values for this component's properties
UDetectionSystemComponentBase::UDetectionSystemComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDetectionSystemComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDetectionSystemComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//如果启动了持续探测Timer 则在此时回收清除
	if (RefreshContinuousDetectionTimerHandle.IsValid())
	{
		UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(GetWorld(), RefreshContinuousDetectionTimerHandle);
	}

	//将监听Actor生成的委托移除
	if (ActorSpawningDelegateHandle.IsValid()) 
	{
		GetWorld()->RemoveOnActorSpawnedHandler(ActorSpawningDelegateHandle);
	}
}


// Called every frame
void UDetectionSystemComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UDetectionSystemComponentBase::Init_Implementation(
	FDetectionConditionConfig newDetectionConditionConfig,
	bool openContinuousDetection)
{
	if (IsInit) return false;
	IsInit = true;

	OwnerActor = GetOwner();

	//获取配置的持续探测条件项目组
	DetectionConditionItems = newDetectionConditionConfig.DetectionConditionItems;

	if (DetectionConditionItems.Num() > 0) 
	{
		//创建持续探测用容器
		DetectionContainers.SetNum(DetectionConditionItems.Num());
		for (size_t i = 0; i < DetectionConditionItems.Num(); i++)
		{
			UDetectionContainer* detectionContainer = NewObject<UDetectionContainer>(this);
			detectionContainer->Init(OwnerActor, i, DetectionConditionItems[i]);
			DetectionContainers[i] = detectionContainer;
		}

		//持续探测Timer 开启或不开启
		InitContinuousDetection(openContinuousDetection, newDetectionConditionConfig.RefreshContinuousDetectionIntervalTime);
	}

	return true;
}

bool UDetectionSystemComponentBase::GetBestTarget(AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo)
{
	if (GetBestTargetByPriority(outDetectionActor, outContainerInfo))
		return true;

	return false;
}

bool UDetectionSystemComponentBase::GetBestTargetByPriority(AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo)
{
	if (IsHaveBestTargetByPriority)
	{
		outDetectionActor = BestTargetByPriority.Actor;
		outContainerInfo = BestTargetByPriorityContainerInfo;
		return true;
	}

	return false;
}

bool UDetectionSystemComponentBase::GetTarget(int detectionContainerIndex, AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo)
{
	if (!DetectionContainers.IsValidIndex(detectionContainerIndex)) return false;

	outContainerInfo = FDetectionContainerInfo(
		DetectionContainers[detectionContainerIndex]->DetectionContainerID, 
		DetectionContainers[detectionContainerIndex]->DetectionConditionItem.Priority
	);

	return DetectionContainers[detectionContainerIndex]->GetTarget(outDetectionActor);
}

bool UDetectionSystemComponentBase::GetTargets(int detectionContainerIndex, TArray<AActor*>& outDetectionActors, FDetectionContainerInfo& outContainerInfo)
{
	if (!DetectionContainers.IsValidIndex(detectionContainerIndex)) return false;

	outContainerInfo = FDetectionContainerInfo(
		DetectionContainers[detectionContainerIndex]->DetectionContainerID,
		DetectionContainers[detectionContainerIndex]->DetectionConditionItem.Priority
	);

	outDetectionActors.SetNum(1);
	return DetectionContainers[detectionContainerIndex]->GetTargets(outDetectionActors);
}

bool UDetectionSystemComponentBase::GetDetectionContainersHaveTarget(TArray<UDetectionContainer*>& outDetectionContainers, int sortType)
{
	if (HaveTargetDetectionContainers.Num() == 0) return false;

	//TODO 之后如果有多重排序方式 可以将排序结果缓存
	//进行排序
	switch (sortType)
	{
	case 0:
		for (int i = 0; i < HaveTargetDetectionContainers.Num(); i++)
		{
			for (int j = 0; j < HaveTargetDetectionContainers.Num() - i - 1; j++)
			{
				int score_j, score_j1;
				score_j = HaveTargetDetectionContainers[j]->GetTargetScore(0);
				score_j1 = HaveTargetDetectionContainers[j + 1]->GetTargetScore(0);

				if (score_j > score_j1)
				{
					UDetectionContainer* containerTemp = HaveTargetDetectionContainers[j];
					HaveTargetDetectionContainers[j] = HaveTargetDetectionContainers[j + 1];
					HaveTargetDetectionContainers[j + 1] = containerTemp;
				}
			}
		}
		break;
	}

	outDetectionContainers = HaveTargetDetectionContainers;

	return true;
}

bool UDetectionSystemComponentBase::DetectionTarget(int detectionContainerIndex, TArray<AActor*>& outDetectionActors, FDetectionContainerInfo& outContainerInfo)
{
	//调用一次探测 并返回探测到的目标
	if (detectionContainerIndex < DetectionContainers.Num())
	{
		//此次探测仅对单个detectionContainer的数据造成影响 其他根据所有detectionContainer的对象筛选出的BestTarget等数据并不会更新 因为这些数据是根据持续探测更新的

		UDetectionContainer* detectionContainer = DetectionContainers[detectionContainerIndex];

		detectionContainer->OnDetection();
		
		//获取探测到的目标组
		detectionContainer->GetTargets(outDetectionActors);

		outContainerInfo = FDetectionContainerInfo(
			detectionContainer->DetectionContainerID,
			detectionContainer->DetectionConditionItem.Priority
		);

		return outDetectionActors.Num() > 0;
	}

	return false;
}

bool UDetectionSystemComponentBase::InitContinuousDetection(bool newOpen, float intervalTime)
{
	//已经存在TimerHandle 清除旧的
	if (RefreshContinuousDetectionTimerHandle.IsValid())
	{
		UKismetSystemLibrary::K2_ClearTimerHandle(GetWorld(), RefreshContinuousDetectionTimerHandle);
	}

	//生成Timer
	if (!RefreshContinuousDetectionTimerHandle.IsValid() && intervalTime > 0)
	{
		RefreshContinuousDetectionIntervalTime = intervalTime;
		RefreshContinuousDetectionTimerHandle = UKismetSystemLibrary::K2_SetTimer(this, "RefreshContinuousDetection", RefreshContinuousDetectionIntervalTime, true);
	}
	
	IsOpenContinuousDetection = true;
	OpenContinuousDetection(newOpen);//开启或关闭

	return true;
}

bool UDetectionSystemComponentBase::OpenContinuousDetection(bool newOpen)
{
	if (!RefreshContinuousDetectionTimerHandle.IsValid() || IsOpenContinuousDetection == newOpen) return false;
	IsOpenContinuousDetection = newOpen;

	if (IsOpenContinuousDetection)
	{
		UKismetSystemLibrary::K2_UnPauseTimerHandle(GetWorld(), RefreshContinuousDetectionTimerHandle);
	}
	else
	{
		UKismetSystemLibrary::K2_PauseTimerHandle(GetWorld(), RefreshContinuousDetectionTimerHandle);

		//清除探测到的目标缓存
		for (auto item : DetectionContainers) 
		{
			item->ClearAllTarget();
		}

		IsHaveBestTargetByPriority = false; BestTargetByPriority = FDetectionObjectInfo();
	}

	return false;
}

bool UDetectionSystemComponentBase::GetDetectionContainers(TArray<UDetectionContainer*>& outDetectionContainers)
{
	outDetectionContainers = DetectionContainers;
	return outDetectionContainers.Num() > 0;
}

bool UDetectionSystemComponentBase::GetDetectionContainer(FName purposeName, UDetectionContainer*& outDetectionContainer)
{
	if (DetectionContainers.Num() == 0) return false;

	for (auto& item : DetectionContainers)
	{
		if (item == nullptr) continue;

		if (item->DetectionConditionItem.PurposeTag == purposeName)
		{
			outDetectionContainer = item;
			return true;
		}
	}

	return false;
}

void UDetectionSystemComponentBase::RefreshContinuousDetection()
{
	if (DetectionContainers.Num() == 0) return;

	HaveTargetDetectionContainers.Empty();

	if(OnContinuousDetectionStart.IsBound())
		OnContinuousDetectionStart.Broadcast();

	//遍历探测目标集合
	for (auto& detectionContainer : DetectionContainers)
	{
		if (detectionContainer == nullptr) continue;

		detectionContainer->OnDetection();

		//Component需要从所有探测项目中筛选出一个最佳的目标
		AActor* targetActor;
		if (detectionContainer->GetTarget(targetActor))
		{
			//确认是否是最佳目标
			FDetectionObjectInfo detectionObjectInfo = FDetectionObjectInfo(targetActor, detectionContainer->DetectionContainerID, detectionContainer->DetectionConditionItem.Priority);
			bool isBestTargetByWeightScore, isBestTargetByPriority;
			CheckBestTarget(detectionContainer, detectionObjectInfo, isBestTargetByWeightScore, isBestTargetByPriority);

			HaveTargetDetectionContainers.Add(detectionContainer);
		}
	}

	if(OnContinuousDetectionEnd.IsBound())
		OnContinuousDetectionEnd.Broadcast();
}

void UDetectionSystemComponentBase::CheckBestTarget(UDetectionContainer* detectionContainer, const FDetectionObjectInfo detectionObjectInfo, bool& isBestTargetByWeightScore, bool& isBestTargetByPriority)
{
	//在持续探测时 从所有探测容器获得的探测目标中 获得一个最佳探测目标

	isBestTargetByPriority = true;
	
	//根据探测条件配置的优先级获得的最佳目标
	if (!IsHaveBestTargetByPriority)
	{
		IsHaveBestTargetByPriority = true;
		BestTargetByPriority = detectionObjectInfo;
		BestTargetByPriorityContainerInfo = FDetectionContainerInfo(detectionContainer->DetectionContainerID, detectionContainer->DetectionConditionItem.Priority);
	}
	else if (detectionObjectInfo.Priority > BestTargetByPriority.Priority)
	{
		BestTargetByPriority = detectionObjectInfo;
		BestTargetByPriorityContainerInfo = FDetectionContainerInfo(detectionContainer->DetectionContainerID, detectionContainer->DetectionConditionItem.Priority);
	}
	else if (detectionObjectInfo.Priority == BestTargetByPriority.Priority)
	{
		//当优先级相等时 确认权重分 和探测的下标ID
		if (detectionObjectInfo.DetectionContainerID < BestTargetByPriority.DetectionContainerID)
		{
			BestTargetByPriority = detectionObjectInfo;
			BestTargetByPriorityContainerInfo = FDetectionContainerInfo(detectionContainer->DetectionContainerID, detectionContainer->DetectionConditionItem.Priority);
		}
	}
	else
		isBestTargetByPriority = false;
}

void UDetectionSystemComponentBase::OnActorDestroy(AActor* DestroyedActor)
{

	DestroyedActor->OnDestroyed.RemoveDynamic(this, &UDetectionSystemComponentBase::OnActorDestroy);
}

#pragma region UDetectionContainer

void UDetectionContainer::Init(
	AActor* newOwnerActor, int newDetectionContainerID, FDetectionConditionItem& newDetectionConditionItem)
{
	OwnerActor = newOwnerActor;
	DetectionContainerID = newDetectionContainerID;
	DetectionConditionItem = newDetectionConditionItem;
}

void UDetectionContainer::OnDetection()
{
	//确认有效性
	if (DetectionConditionItem.DetectionCondition.EnvQuery == nullptr) return;
	if (OwnerActor == nullptr || !UKismetSystemLibrary::IsValid(OwnerActor)) return;

	// -- 重置状态 清空数据
	
	//清空筛选出的目标缓存
	TargetActors.Empty();
	TargetActors.Empty();
	IsHaveTargetObject = false;
	
	// -- 运行EQS并收集数据

	//运行EQS
	if (EnvQueryInstance == nullptr)
	{
		EnvQueryInstance = UEnvQueryManager::RunEQSQuery(this, DetectionConditionItem.DetectionCondition.EnvQuery, OwnerActor, EEnvQueryRunMode::Type::AllMatching, nullptr);
	}
	else
	{
		FEnvQueryRequest QueryRequest(DetectionConditionItem.DetectionCondition.EnvQuery, OwnerActor);
		EnvQueryInstance->RunQuery(EEnvQueryRunMode::Type::AllMatching, QueryRequest);
	}

	if (EnvQueryInstance == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), "UDetectionContainer->OnDetection  EnvQueryInstance is null!",
			true, true, FLinearColor::Red, 10.f);
		return;
	}

	// -- 更新信息

	//获取所有探测到的目标
	//Tips 实际上直接获取是获取不到此次运行EQS所获得的数据的 而是获取到之前运行完成后的缓存 因为时持续探测所以直接写在这没什么问题
	EnvQueryInstance->GetQueryResultsAsActors(InRangeActors);
	
	//获取指定数量的目标
	TargetActors.Empty();
	if (InRangeActors.Num() > 0)
	{
		TargetActor = InRangeActors[0];
		for (int i = 0; i < DetectionConditionItem.DetectionCondition.Amount; i++)
		{
			if (!InRangeActors.IsValidIndex(i)) break;
			TargetActors.Add(InRangeActors[i]);
		}

		IsHaveTargetObject = true;
	}
}

bool UDetectionContainer::GetTarget(AActor*& outDetectionActor)
{
	if (!IsHaveTargetObject) return false;

	outDetectionActor = TargetActor;

	return true;
}

bool UDetectionContainer::GetTargets(TArray<AActor*>& outDetectionActors)
{
	if (!IsHaveTargetObject) return false;

	outDetectionActors = TargetActors;

	return true;
}

int UDetectionContainer::GetTargetScore(int itemIndex)
{
	if(EnvQueryInstance == nullptr) return -1;
	return EnvQueryInstance->GetItemScore(itemIndex);
}

void UDetectionContainer::ClearAllTarget()
{
	IsHaveTargetObject = false;

	InRangeActors.Empty();
	TargetActors.Empty();
	TargetActor = nullptr;
}

void UDetectionContainer::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	
}

#pragma endregion