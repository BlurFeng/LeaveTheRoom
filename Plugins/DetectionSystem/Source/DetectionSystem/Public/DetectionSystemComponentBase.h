// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>

#include "Delegates/IDelegateInstance.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DetectionSystemComponentBase.generated.h"

//无参数
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMDDetectionAction);

/*探测容器，和探测条件配置条目对应，用于存储探测条件配置，对目标进行筛选并存储*/
UCLASS(Blueprintable, ClassGroup = (DetectionSystem))
class DETECTIONSYSTEM_API UDetectionContainer : public UObject
{
	GENERATED_BODY()

public:

	/*探测容器ID 对应探测条件配置Array的Index和 探测容器Array的Index*/
	UPROPERTY(BlueprintReadOnly, Category = "DetectionSystem|DetectionContainer")
		int DetectionContainerID;

	/*探测条件配置*/
	UPROPERTY(BlueprintReadOnly, Category = "DetectionSystem|DetectionContainer")
		FDetectionConditionItem DetectionConditionItem;

	/*探测容器参与的探测目标集合 在初始化时加入 数量和探测目标条件中的类型数量相同*/
	UPROPERTY()
		TArray<int> DetectionTargetGatherIndexs;

	

	/**
	 初始化
	 * @param newOwnerActor 所有者Actor
	 * @param newDetectionContainerID 所属探测容器ID
	 * @param newDetectionConditionItem 探测条件配置项目
	*/
	UFUNCTION()
		void Init(AActor* newOwnerActor, int newDetectionContainerID, FDetectionConditionItem& newDetectionConditionItem);

	/*执行一次探测 并更新数据*/
	UFUNCTION()
		void OnDetection();

	/**
	 获取最佳目标
	 * @param outDetectionActor - 探测到的目标Actor
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|DetectionContainer")
		bool GetTarget(AActor*& outDetectionActor);

	/**
	 获取最佳目标组
	 * @param outDetectionActors - 探测到的目标Actor组
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|DetectionContainer")
		bool GetTargets(TArray<AActor*>& outDetectionActors);

	/*获取目标分数*/
	UFUNCTION()
		int GetTargetScore(int itemIndex);

	/**
	 * 清空所有目标
	*/
	UFUNCTION()
		void ClearAllTarget();

	/**
	 * @brief 当EQS运行完成时
	*/
	UFUNCTION()
		void OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

protected:

	/*在范围内的所有Actors*/
	UPROPERTY(BlueprintReadOnly, Category = "DetectionSystem|DetectionContainer")
		TArray<AActor*> InRangeActors;

	/*筛选出的目标Actor数组*/
	UPROPERTY()
		TArray<AActor*> TargetActors;

private:

	/*所有者Actor*/
	UPROPERTY()
		AActor* OwnerActor;

	/*是否有探测到目标*/
	UPROPERTY()
		bool IsHaveTargetObject;

	/*筛选出的目标Actor*/
	UPROPERTY()
		AActor* TargetActor;

	/*EQS实例*/
	UPROPERTY()
		UEnvQueryInstanceBlueprintWrapper* EnvQueryInstance;
};


/*探测系统功能组件 添加到Actor以获得探测功能*/
UCLASS(Blueprintable, ClassGroup=(DetectionSystem), meta=(BlueprintSpawnableComponent))
class DETECTIONSYSTEM_API UDetectionSystemComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDetectionSystemComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*所有者Actor*/
	UPROPERTY(BlueprintReadOnly, Category = "DetectionSystem|Default")
		AActor* OwnerActor;

	/*刷新持续探测间隔时间*/
	UPROPERTY(BlueprintReadOnly, Category = "DetectionSystem|Default")
		float RefreshContinuousDetectionIntervalTime = 0.1f;

	/*当持续探测刷新开始时*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "DetectionSystem|Default")
		FMDDetectionAction OnContinuousDetectionStart;

	/*当持续探测刷新结束时*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable, Category = "DetectionSystem|Default")
		FMDDetectionAction OnContinuousDetectionEnd;

	/*开启调试*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DetectionSystem|Default")
		bool EnableDebug;

	/*
	初始化
	@param newDetectionConditionConfig - 探测条件配置文件
	@param openContinuousDetection - 是否启动持续探测
	@return 是否成功执行初始化
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|Default")
		bool Init(FDetectionConditionConfig newDetectionConditionConfig, bool openContinuousDetection = true);
	bool Init_Implementation(FDetectionConditionConfig newDetectionConditionConfig, bool openContinuousDetection = true);

	/**
	 * 获取最佳目标
	 * @param outDetectionActor - 探测到的目标Actor
	 * @return 是否有目标
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool GetBestTarget(AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo);

	/**
	 * 获取最佳目标 （通过Priority优先级(越小优先级越高，优先级相同时判断DetectionContainerID探测条件列表中的下标）
	 * @param outDetectionActor - 探测到的目标Actor
	 * @return 是否有目标
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
	bool GetBestTargetByPriority(AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo);

	/**
	 获取目标 某个探测容器的目标
	 * @param detectionContainerIndex - 探测容器下标 对应探测条件Datatable中的顺序
	 * @param outDetectionActor - 探测到的目标Actor
	 * @return 是否有目标
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool GetTarget(int detectionContainerIndex, AActor*& outDetectionActor, FDetectionContainerInfo& outContainerInfo);

	/**
	 获取目标组 某个探测容器的目标
	 * @param detectionContainerIndex - 探测容器下标 对应探测条件Datatable中的顺序
	 * @param outDetectionActors - 探测到的目标Actor组
	 * @return 是否有目标
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool GetTargets(int detectionContainerIndex, TArray<AActor*>& outDetectionActors, FDetectionContainerInfo& outContainerInfo);

	/**
	 获取所有有探测到目标的容器
	 * @param outDetectionContainers - 探测到的目标Actor组
	 * @param sortType - 对容器的排序方式 0=按最佳目标分数排序
	 * @return 是否有目标
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool GetDetectionContainersHaveTarget(TArray<UDetectionContainer*>& outDetectionContainers, int sortType = 0);

	/**
	 * 执行一次探测 并返回目标对象
	 * @param detectionContainerIndex 探测容器下标 对应探测条件Datatable中的顺序
	 * @param outActor 目标对象组
	 * @return 是否获得了目标对象
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool DetectionTarget(int detectionContainerIndex, TArray<AActor*>& outDetectionActors, FDetectionContainerInfo& outContainerInfo);

	/**
	 * 初始化持续探测Timer 当已经存在时会清楚旧的生成新的
	 * @param newOpen 开关
	 * @param intervalTime 间隔时间 仅在首次打开初始化成功时用到
	 * @return
	*/
	UFUNCTION()
		bool InitContinuousDetection(bool newOpen, float intervalTime = -1);

	/**
	 * 打开或关闭持续探测Timer
	 * @param newOpen 开关
	 * @return 是否成功设置
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool OpenContinuousDetection(bool newOpen);

	/**
	 * 获取所有探测容器 探测容器数量和探测条件配置Config的数量一致
	 * @param outDetectionContainers 探测容器数组
	 * @return 是否有一个以上的探测容器
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
	bool GetDetectionContainers(TArray<UDetectionContainer*>& outDetectionContainers);

	/**
	 * 获取探测容器 根据EDetectionPurpose探测用途枚举
	 * @param detectionPurpose 探测用途
	 * @return 是否有探测容器
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|Default")
		bool GetDetectionContainer(FName purposeTag, UDetectionContainer*& outDetectionContainer);

private:

	/*是否初始化*/
	UPROPERTY()
		bool IsInit;

	/*是否拥有最佳目标 通过优先级(越小优先级越高，优先级相同时判断DetectionContainerID探测条件列表中的下标*/
	UPROPERTY()
		bool IsHaveBestTargetByPriority;

	/*最佳目标 通过优先级(越小优先级越高，优先级相同时判断探测条件列表中的下标*/
	UPROPERTY()
		FDetectionObjectInfo BestTargetByPriority;

	/*最佳目标 通过优先级 对应的探测容器*/
	UPROPERTY()
		FDetectionContainerInfo BestTargetByPriorityContainerInfo;

	/*持续刷新探测TimerHandle*/
	UPROPERTY()
		FTimerHandle RefreshContinuousDetectionTimerHandle;

	/*持续探测条件项目组*/
	UPROPERTY()
		TArray<FDetectionConditionItem> DetectionConditionItems;

	/*持续探测用容器 和 持续探测条件项目组 对应*/
	UPROPERTY()
		TArray<UDetectionContainer*> DetectionContainers;

	/*持续探测是否启动*/
	UPROPERTY()
		bool IsOpenContinuousDetection;

	/*当前拥有目标的探测容器*/
	UPROPERTY()
		TArray<UDetectionContainer*> HaveTargetDetectionContainers;

	FDelegateHandle ActorSpawningDelegateHandle;

	/*刷新持续探测*/
	UFUNCTION()
		void RefreshContinuousDetection();

	/*持续探测时 确认并筛选出最佳目标*/
	UFUNCTION()
		void CheckBestTarget(UDetectionContainer* detectionContainer, const FDetectionObjectInfo detectionObjectInfo, bool& isBestTargetByWeightScore, bool& isBestTargetByPriority);

	UFUNCTION()
		void OnActorDestroy(AActor* DestroyedActor);

};