// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include "Engine/DataTable.h"
#include "EnvironmentQuery/EnvQuery.h"

#include "CoreMinimal.h"
#include "Engine.h"
#include "DetectionSystemStaticData.generated.h"

/*
* 探测条件配置
* 用于探测的异步方法DetectionTarget
*/
USTRUCT(BlueprintType)
struct FDetectionCondition : public FTableRowBase
{
	GENERATED_BODY()

public:

	/*使用的EQS模板*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UEnvQuery* EnvQuery;

	/*获取目标数量 (-1获取所有)*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Amount = 1;
};

/*
* 探测条件配置项目
* 在DetectionSystemComponent中使用
*/
USTRUCT(BlueprintType)
struct FDetectionConditionItem
{
	GENERATED_BODY()

public:

	/*使用的EQS模板*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FDetectionCondition DetectionCondition;

	/*优先级 用于获取唯一最佳对象时进行判断 数值越低优先级越高*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Priority = 1;

	/*用途Tag 之后可通过此Tag获取某一个探测容器*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName PurposeTag;
};

/*
* 探测项目配置
* 用于DetectionSystemComponent的初始化
*/
USTRUCT(BlueprintType)
struct FDetectionConditionConfig : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RefreshContinuousDetectionIntervalTime = 0.1f;

	/*探测条件项目组 可配置多个*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDetectionConditionItem> DetectionConditionItems;
};

/*
* 探测项目配置库
* 在调用DetectionTarget方法时 要求传入FDetectionCondition
* 可以在此DataTable中预先配置好
*/
USTRUCT(BlueprintType)
struct FDetectionConditionLibrary : public FTableRowBase
{
	GENERATED_BODY()

public:
	/*探测条件项目组 可配置多个*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDetectionCondition> DetectionConditions;
};



/*
*探测距离配置
* 为探测对象配置最远探测距离
* 可以在此DataTable中预先配置好
*/
USTRUCT(BlueprintType)
struct FItemMaxDetectionDistance: public FTableRowBase
{
	GENERATED_BODY()

public:
	/*探测条件项目组 可配置多个*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxDistance;
};