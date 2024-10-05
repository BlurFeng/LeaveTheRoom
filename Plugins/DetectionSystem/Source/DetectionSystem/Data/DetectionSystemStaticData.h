// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQuery.h"

#include "CoreMinimal.h"
#include "Engine.h"
#include "DetectionSystemStaticData.generated.h"

/*
* 探测条件配置。
*/
USTRUCT(BlueprintType)
struct FDetectionCondition : public FTableRowBase
{
	GENERATED_BODY()

public:

	/**
	 * 使用的EQS测试模板。
	 * Used EQS test template.
	 * 使用されるEQSテストテンプレート。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UEnvQuery* EnvQuery;

	/**
	 * 需要获取目标数量。 (-1为获取所有)
	 * Need to get the number of targets. (-1 means to get all.)
	 * ターゲットの数を取得する必要があります。(-1はすべてを取得することを意味します。)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Amount = 1;
};

/*
* 探测条件配置项目。
*/
USTRUCT(BlueprintType)
struct FDetectionConditionItem
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDetectionCondition DetectionCondition;

	/**
	 * 优先级，数值越高优先级越高。
	 * Priority, the higher the value, the higher the priority.
	 * 優先度、数値が高いほど優先度も高くなります。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Priority = 1;

	/**
	 * 用途Tag，可用于获取指定Tag的DetectionContainer。
	 * Usage Tag, used to retrieve the DetectionContainer with the specified Tag.
	 * 用途タグ、指定されたタグのDetectionContainerを取得するために使用されます。
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PurposeTag;
};

/*
* 探测项目配置，用于DetectionSystemComponent的初始化。
* Detection item configuration, used for initializing the DetectionSystemComponent.
* 検出アイテムの設定、DetectionSystemComponentの初期化に使用されます。
*/
USTRUCT(BlueprintType)
struct FDetectionConditionConfig : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RefreshContinuousDetectionIntervalTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDetectionConditionItem> DetectionConditionItems;
};

///*
//* 探测项目配置库
//* 在调用DetectionTarget方法时 要求传入FDetectionCondition
//* 可以在此DataTable中预先配置好
//*/
//USTRUCT(BlueprintType)
//struct FDetectionConditionLibrary : public FTableRowBase
//{
//	GENERATED_BODY()
//
//public:
//	/*探测条件项目组 可配置多个*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		TArray<FDetectionCondition> DetectionConditions;
//};
//
//
//
///*
//* 探测距离配置
//* 为探测对象配置最远探测距离
//* 可以在此DataTable中预先配置好
//*/
//USTRUCT(BlueprintType)
//struct FItemMaxDetectionDistance: public FTableRowBase
//{
//	GENERATED_BODY()
//
//public:
//	/*探测条件项目组 可配置多个*/
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		FName ItemID;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//		float MaxDistance;
//};