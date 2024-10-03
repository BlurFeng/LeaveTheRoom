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
	FDetectionConditionConfig NewDetectionConditionConfig,
	bool OpenContinuousDetection)
{
	if (IsInit) return false;
	IsInit = true;

	OwnerActor = GetOwner();

	//获取配置的持续探测条件项目组
	DetectionConditionItems = NewDetectionConditionConfig.DetectionConditionItems;

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
		InitContinuousDetection(OpenContinuousDetection, NewDetectionConditionConfig.RefreshContinuousDetectionIntervalTime);
	}

	return true;
}

bool UDetectionSystemComponentBase::GetBestTarget(AActor*& OutDetectionActor, FDetectionContainerData& OutContainerData)
{
	if (GetBestTargetByPriority(OutDetectionActor, OutContainerData))
		return true;

	return false;
}

bool UDetectionSystemComponentBase::GetBestTargetByPriority(AActor*& OutDetectionActor, FDetectionContainerData& OutContainerData)
{
	if (IsHaveBestTargetByPriority)
	{
		OutDetectionActor = BestTargetByPriority.Actor;
		OutContainerData = BestTargetByPriorityContainerData;
		return true;
	}

	return false;
}

bool UDetectionSystemComponentBase::GetTarget(int DetectionContainerIndex, AActor*& OutDetectionActor, FDetectionContainerData& OutContainerData)
{
	if (!DetectionContainers.IsValidIndex(DetectionContainerIndex)) return false;

	OutContainerData = FDetectionContainerData(
		DetectionContainers[DetectionContainerIndex]->DetectionContainerID, 
		DetectionContainers[DetectionContainerIndex]->DetectionConditionItem.Priority
	);

	return DetectionContainers[DetectionContainerIndex]->GetTarget(OutDetectionActor);
}

bool UDetectionSystemComponentBase::GetTargets(int DetectionContainerIndex, TArray<AActor*>& OutDetectionActors, FDetectionContainerData& OutContainerData)
{
	if (!DetectionContainers.IsValidIndex(DetectionContainerIndex)) return false;

	OutContainerData = FDetectionContainerData(
		DetectionContainers[DetectionContainerIndex]->DetectionContainerID,
		DetectionContainers[DetectionContainerIndex]->DetectionConditionItem.Priority
	);

	OutDetectionActors.SetNum(1);
	return DetectionContainers[DetectionContainerIndex]->GetTargets(OutDetectionActors);
}

bool UDetectionSystemComponentBase::GetDetectionContainersHaveTarget(TArray<UDetectionContainer*>& OutDetectionContainers, int SortType)
{
	if (HaveTargetDetectionContainers.Num() == 0) return false;

	//TODO 之后如果有多重排序方式 可以将排序结果缓存
	//进行排序
	switch (SortType)
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

	OutDetectionContainers = HaveTargetDetectionContainers;

	return true;
}

bool UDetectionSystemComponentBase::DetectionTarget(int DetectionContainerIndex, TArray<AActor*>& OutDetectionActors, FDetectionContainerData& OutContainerData)
{
	//调用一次探测 并返回探测到的目标
	if (DetectionContainerIndex < DetectionContainers.Num())
	{
		//此次探测仅对单个detectionContainer的数据造成影响 其他根据所有detectionContainer的对象筛选出的BestTarget等数据并不会更新 因为这些数据是根据持续探测更新的

		UDetectionContainer* detectionContainer = DetectionContainers[DetectionContainerIndex];

		detectionContainer->OnDetection();
		
		//获取探测到的目标组
		detectionContainer->GetTargets(OutDetectionActors);

		OutContainerData = FDetectionContainerData(
			detectionContainer->DetectionContainerID,
			detectionContainer->DetectionConditionItem.Priority
		);

		return OutDetectionActors.Num() > 0;
	}

	return false;
}

bool UDetectionSystemComponentBase::InitContinuousDetection(bool NewOpen, float IntervalTime)
{
	//已经存在TimerHandle 清除旧的
	if (RefreshContinuousDetectionTimerHandle.IsValid())
	{
		UKismetSystemLibrary::K2_ClearTimerHandle(GetWorld(), RefreshContinuousDetectionTimerHandle);
	}

	//生成Timer
	if (!RefreshContinuousDetectionTimerHandle.IsValid() && IntervalTime > 0)
	{
		RefreshContinuousDetectionIntervalTime = IntervalTime;
		RefreshContinuousDetectionTimerHandle = UKismetSystemLibrary::K2_SetTimer(this, "RefreshContinuousDetection", RefreshContinuousDetectionIntervalTime, true);
	}
	
	IsOpenContinuousDetection = true;
	OpenContinuousDetection(NewOpen);//开启或关闭

	return true;
}

bool UDetectionSystemComponentBase::OpenContinuousDetection(bool NewOpen)
{
	if (!RefreshContinuousDetectionTimerHandle.IsValid() || IsOpenContinuousDetection == NewOpen) return false;
	IsOpenContinuousDetection = NewOpen;

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

		IsHaveBestTargetByPriority = false; BestTargetByPriority = FDetectionObjectData();
	}

	return false;
}

bool UDetectionSystemComponentBase::GetDetectionContainers(TArray<UDetectionContainer*>& OutDetectionContainers)
{
	OutDetectionContainers = DetectionContainers;
	return OutDetectionContainers.Num() > 0;
}

bool UDetectionSystemComponentBase::GetDetectionContainer(FName PurposeName, UDetectionContainer*& OutDetectionContainer)
{
	if (DetectionContainers.Num() == 0) return false;

	for (auto& item : DetectionContainers)
	{
		if (item == nullptr) continue;

		if (item->DetectionConditionItem.PurposeTag == PurposeName)
		{
			OutDetectionContainer = item;
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

	bool GetTarget = false;
	FDetectionObjectData BestTargetCached = FDetectionObjectData(BestTargetByPriority);

	//遍历探测目标集合
	for (auto& DetectionContainer : DetectionContainers)
	{
		if (DetectionContainer == nullptr) continue;

		DetectionContainer->OnDetection();

		//Component需要从所有探测项目中筛选出一个最佳的目标
		AActor* targetActor;
		if (DetectionContainer->GetTarget(targetActor))
		{
			GetTarget = true;

			//确认是否是最佳目标
			FDetectionObjectData detectionObjectInfo = FDetectionObjectData(targetActor, DetectionContainer->DetectionContainerID, DetectionContainer->DetectionConditionItem.Priority);
			bool isBestTargetByWeightScore, isBestTargetByPriority;
			CheckBestTarget(DetectionContainer, detectionObjectInfo, isBestTargetByWeightScore, isBestTargetByPriority);

			HaveTargetDetectionContainers.Add(DetectionContainer);
		}
	}

	//没有目标
	if (GetTarget == false) 
	{
		//重置最佳目标数据
		IsHaveBestTargetByPriority = false;
		BestTargetByPriority = FDetectionObjectData();
	}

	if (!UDetectionSystemUtility::EqualsDetectionObjectData(BestTargetCached, BestTargetByPriority))
	{
		if (OnBestTargetChange.IsBound())
			OnBestTargetChange.Broadcast(IsHaveBestTargetByPriority, BestTargetByPriority);
	}

	if(OnContinuousDetectionEnd.IsBound())
		OnContinuousDetectionEnd.Broadcast();
}

void UDetectionSystemComponentBase::CheckBestTarget(UDetectionContainer* DetectionContainer, const FDetectionObjectData DetectionObjectInfo, bool& IsBestTargetByWeightScore, bool& IsBestTargetByPriority)
{
	//在持续探测时 从所有探测容器获得的探测目标中 获得一个最佳探测目标

	IsBestTargetByPriority = true;
	
	//根据探测条件配置的优先级获得的最佳目标
	if (!IsHaveBestTargetByPriority)
	{
		IsHaveBestTargetByPriority = true;
		BestTargetByPriority = DetectionObjectInfo;
		BestTargetByPriorityContainerData = FDetectionContainerData(DetectionContainer->DetectionContainerID, DetectionContainer->DetectionConditionItem.Priority);
	}
	else if (DetectionObjectInfo.Priority > BestTargetByPriority.Priority)
	{
		BestTargetByPriority = DetectionObjectInfo;
		BestTargetByPriorityContainerData = FDetectionContainerData(DetectionContainer->DetectionContainerID, DetectionContainer->DetectionConditionItem.Priority);
	}
	else if (DetectionObjectInfo.Priority == BestTargetByPriority.Priority)
	{
		//同一个探测容器，但目标可能变化了
		if (DetectionObjectInfo.DetectionContainerID == BestTargetByPriority.DetectionContainerID) 
		{
			BestTargetByPriority = DetectionObjectInfo;
		}
		//当优先级相等时 确认权重分 和探测的下标ID
		else if (DetectionObjectInfo.DetectionContainerID < BestTargetByPriority.DetectionContainerID)
		{
			BestTargetByPriority = DetectionObjectInfo;
			BestTargetByPriorityContainerData = FDetectionContainerData(DetectionContainer->DetectionContainerID, DetectionContainer->DetectionConditionItem.Priority);
		}
	}
	else
		IsBestTargetByPriority = false;
}

void UDetectionSystemComponentBase::OnActorDestroy(AActor* DestroyedActor)
{

	DestroyedActor->OnDestroyed.RemoveDynamic(this, &UDetectionSystemComponentBase::OnActorDestroy);
}

#pragma region UDetectionContainer

void UDetectionContainer::Init(
	AActor* NewOwnerActor, int NewDetectionContainerID, FDetectionConditionItem& NewDetectionConditionItem)
{
	OwnerActor = NewOwnerActor;
	DetectionContainerID = NewDetectionContainerID;
	DetectionConditionItem = NewDetectionConditionItem;
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
	EnvQueryInstance->GetQueryResultsAsActors(QueryResults);
	
	//获取指定数量的目标
	TargetActors.Empty();
	if (QueryResults.Num() > 0)
	{
		TargetActor = QueryResults[0];
		for (int i = 0; i < DetectionConditionItem.DetectionCondition.Amount; i++)
		{
			if (!QueryResults.IsValidIndex(i)) break;
			TargetActors.Add(QueryResults[i]);
		}

		IsHaveTargetObject = true;
	}
}

bool UDetectionContainer::GetTarget(AActor*& OutDetectionActor)
{
	if (!IsHaveTargetObject) return false;

	OutDetectionActor = TargetActor;

	return true;
}

bool UDetectionContainer::GetTargets(TArray<AActor*>& OutDetectionActors)
{
	if (!IsHaveTargetObject) return false;

	OutDetectionActors = TargetActors;

	return true;
}

int UDetectionContainer::GetTargetScore(int ItemIndex)
{
	if(EnvQueryInstance == nullptr) return -1;
	return EnvQueryInstance->GetItemScore(ItemIndex);
}

void UDetectionContainer::ClearAllTarget()
{
	IsHaveTargetObject = false;

	QueryResults.Empty();
	TargetActors.Empty();
	TargetActor = nullptr;
}

void UDetectionContainer::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	
}

#pragma endregion