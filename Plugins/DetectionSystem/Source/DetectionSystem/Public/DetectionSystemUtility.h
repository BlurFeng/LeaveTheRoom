// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DetectionSystemUtility.generated.h"

/*探测系统工具类*/
UCLASS()
class DETECTIONSYSTEM_API UDetectionSystemUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * @brief - 探测目标
	 * @param Querier - 查询者 发起这次探测的对象
	 * @param DetectionConditionItem - 探测条件(可直接Make或使用配置表中数据)
	 * @param IgnoreActors - 忽略的Actor
	 * @param EnvQueryInstance - EQS运行后生成实例 结果由OnQueryFinished多播返回
	*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "DetectionSystem|Utility")
		static bool DetectionTarget(
			UObject* WorldContextObject, UObject* Querier, FDetectionCondition DetectionCondition,
			TArray<AActor*> IgnoreActors, UEnvQueryInstanceBlueprintWrapper*& EnvQueryInstance);

	/*
	在场景中绘制探测范围 圆底倒圆锥
	@param originPointPos - 探测源点 位置信息
	@param originPointReferenceDir - 探测源点 参照方向
	@param originPointQuat - 探测源点 旋转四元数
	@param radius - 探测范围半径
	@param angle - 探测范围角度限制
	@param originPointPosOffset - 探测源点位置偏移 偏移会根据源点的坐标系
	@param numSides - 锥形边数量 最小为4
	@param lifeTime - 绘制图形存活时间
	@param thickness - 绘制线条厚度
	*/
	UFUNCTION(BlueprintCallable, Category = "DetectionSystem|DetectionObject", meta = (WorldContext = "worldContextObject"))
		static void DetectionDebugDrawInvertedCone(
			const UObject* WorldContextObject,
			FVector OriginPointPos, FVector OriginPointReferenceDir, FRotator OriginPointRotator, float Radius, float Angle, FVector OriginPointPosOffset,
			int32 NumSides = 12, float LifeTime = 1.f, float Thickness = 0.f);

	/*
	获得圆锥底部圆形上的点位置信息
	@param drawAngle - 圆锥的角度
	@param NumSides - 获取的点数量
	*/
	UFUNCTION()
		static TArray<FVector> GetConeVertsInDegrees(FVector const& Origin, FVector const& Direction, float Length, const float Angle, int32 NumSides);

	UFUNCTION(BlueprintPure, Category = "DetectionSystem|DetectionObjectData")
		static bool EqualsDetectionObjectData(const FDetectionObjectData& MyDataReferenceA, const FDetectionObjectData& MyDataReferenceB);
};
