// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include "Engine/DataTable.h"

#include "CoreMinimal.h"
#include "DetectionSystemDynamicData.generated.h"

/**
 * 探测到的对象数据。
 * Data of detected objects.
 * 検出されたオブジェクトのデータ。
 */
USTRUCT(BlueprintType)
struct DETECTIONSYSTEM_API FDetectionObjectData
{
	GENERATED_BODY()

		FDetectionObjectData() { Actor = nullptr; DetectionContainerID = Priority = 0; }
	FDetectionObjectData(AActor* newActor, int newDetectionContainerID, int newPriority)
	{
		Actor = newActor;
		DetectionContainerID = newDetectionContainerID;
		Priority = newPriority;
	}

public:

	UPROPERTY(BlueprintReadOnly)
		AActor* Actor;
	
	/**
	 * 数据来源于所属的DetectionContainerData。
	 * The data comes from the associated DetectionContainerData.
	 * データは関連するDetectionContainerDataから取得されます。
	 */
	UPROPERTY(BlueprintReadOnly)
		int DetectionContainerID;

	/**
	 * 数据来源于所属的DetectionContainerData。
	 * The data comes from the associated DetectionContainerData.
	 * データは関連するDetectionContainerDataから取得されます。
	 */
	UPROPERTY(BlueprintReadOnly)
		int Priority;
};

/**/
USTRUCT(BlueprintType)
struct DETECTIONSYSTEM_API FDetectionContainerData
{
	GENERATED_BODY()

		FDetectionContainerData() { DetectionContainerID = Priority = 0; }
	FDetectionContainerData(int newDetectionContainerID, int newPriority)
	{
		DetectionContainerID = newDetectionContainerID;
		Priority = newPriority;
	}

public:

	/**
	 * 探测容器ID，和探测条件配置Array的Index和探测容器Array的Index相同。
	 * The detected container ID corresponds to the index of the detection condition configuration array and the index of the detection container array.
	 * 検出されたコンテナIDは、検出条件設定配列のインデックスと検出コンテナ配列のインデックスが同じです。
	 */
	UPROPERTY(BlueprintReadOnly)
		int DetectionContainerID;

	/**
	 * 优先级，数值越高优先级越高。
	 * Priority, the higher the value, the higher the priority.
	 * 優先度、数値が高いほど優先度も高くなります。
	 */
	UPROPERTY(BlueprintReadOnly)
		int Priority;
};