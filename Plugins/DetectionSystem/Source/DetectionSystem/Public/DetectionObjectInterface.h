// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DetectionObjectInterface.generated.h"

//20211012 Winhoo:接口不再必要 可以被删除
// 之前要求被探测目标必须继承此接口才能被探测到，现在使用EQS探测。等旧项目继承了此接口的内容不再需要时，一起删除此脚本
UINTERFACE(Blueprintable, MinimalAPI)
class UDetectionObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 被探测的对象实现 和探测组件交互使用的功能
 */
class DETECTIONSYSTEM_API IDetectionObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * 是否是有效的目标 最优先确认（在进行其他条件判断前执行
	 * @return 是否有效目标
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|DetectionObjectInterface")
		bool IsValidTarget();
	virtual bool IsValidTarget_Implementation();

	/**
	获取位置 大部分情况下Actor的Location并不准确 Actor.GetBounds获取的中心点也不准确
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|DetectionObjectInterface")
		FVector GetDetectionLocation();
	virtual FVector GetDetectionLocation_Implementation();

	/**
	 * 获取目标的FGameplayTagContainer 用于条件检测
	 * @return 目标拥有的Tags
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|DetectionObjectInterface")
		bool GetGameplayTags(TArray<FName>& OutGameplayTags);
	virtual bool GetGameplayTags_Implementation(TArray<FName>& OutGameplayTags);

	/**
	 * 能否被选择 最迟确认（在确认其他条件都符合后执行
	 * @param actor - 探测者
	 * @return 是否能被选择
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|DetectionObjectInterface")
		bool IsCanSelect(AActor* Actor);
	virtual bool IsCanSelect_Implementation(AActor* Actor);

	/**
	 * 获取几何范围参数
	 * @param RangeType 范围类型 不同范围类型需要提供的参数格式不同
	 * @param param 参数 用'|'号隔离
	 * @return
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "DetectionSystem|DetectionObjectInterface")
		bool GetGeometryRangeParam(EDetectionRangeType& RangeType, FString& Param);
	virtual bool GetGeometryRangeParam_Implementation(EDetectionRangeType& RangeType, FString& Param);
};
