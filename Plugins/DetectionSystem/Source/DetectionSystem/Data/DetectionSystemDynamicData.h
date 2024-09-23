// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include "Engine/DataTable.h"

#include "CoreMinimal.h"
#include "DetectionSystemDynamicData.generated.h"

/*探测到的对象*/
USTRUCT(BlueprintType)
struct DETECTIONSYSTEM_API FDetectionObjectInfo
{
	GENERATED_BODY()

		FDetectionObjectInfo() { Actor = nullptr; DetectionContainerID = Priority = 0; }
	FDetectionObjectInfo(AActor* newActor, int newDetectionContainerID, int newPriority)
	{
		Actor = newActor;
		DetectionContainerID = newDetectionContainerID;
		Priority = newPriority;
	}

public:

	UPROPERTY()
		AActor* Actor;

	/*探测容器ID 对应探测条件配置Array的Index和 探测容器Array的Index*/
	UPROPERTY()
		int DetectionContainerID;

	/*优先级 用于获取唯一最佳对象时进行判断 数值越高优先级越高（和FDetectionConditionItem中的优先级对应）*/
	UPROPERTY()
		int Priority;
};

/*探测到的对象*/
USTRUCT(BlueprintType)
struct DETECTIONSYSTEM_API FDetectionContainerInfo
{
	GENERATED_BODY()

		FDetectionContainerInfo() { DetectionContainerID = Priority = 0; }
	FDetectionContainerInfo(int newDetectionContainerID, int newPriority)
	{
		DetectionContainerID = newDetectionContainerID;
		Priority = newPriority;
	}

public:

	/*探测容器ID 对应探测条件配置Array的Index和 探测容器Array的Index*/
	UPROPERTY(BlueprintReadOnly)
		int DetectionContainerID;

	/*优先级 用于获取唯一最佳对象时进行判断 数值越高优先级越高（和FDetectionConditionItem中的优先级对应）*/
	UPROPERTY(BlueprintReadOnly)
		int Priority;
};