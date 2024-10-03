// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionSystemUtility.h"

#include <DetectionSystem/Data/DetectionSystemEnum.h>
#include <DetectionSystem/Data/DetectionSystemStaticData.h>
#include <DetectionSystem/Data/DetectionSystemDynamicData.h>
#include "DetectionObjectInterface.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnvironmentQuery/EnvQueryManager.h"

bool UDetectionSystemUtility::DetectionTarget(
	UObject* WorldContextObject, UObject* Querier, FDetectionCondition DetectionCondition,
	TArray<AActor*> IgnoreActors, UEnvQueryInstanceBlueprintWrapper*& EnvQueryInstance)
{
	EnvQueryInstance = nullptr;

	if (Querier == nullptr) return false;
	if (DetectionCondition.EnvQuery == nullptr) return false;

	UWorld* world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (world == nullptr) return false;

	// -- 运行EQS并收集数据

	//运行EQS
	EnvQueryInstance = 
		 UEnvQueryManager::RunEQSQuery(WorldContextObject, DetectionCondition.EnvQuery, Querier, EEnvQueryRunMode::Type::AllMatching, nullptr);

	if (EnvQueryInstance == nullptr)
	{
		UKismetSystemLibrary::PrintString(WorldContextObject, "UDetectionSystemUtility::DetectionTarget  EnvQueryInstance is null!",
			true, true, FLinearColor::Red, 10.f);
		return false;
	}
	
	//EQS运行后生成实例 结果不是立即生成的 由OnQueryFinished多播返回
	// -- 更新信息

	////获取所有探测到的目标
	//TArray<AActor*> InRangeActors;
	//EnvQueryInstance->GetQueryResultsAsActors(InRangeActors);

	////获取指定数量的目标
	//TargetActors.Empty();
	//if (InRangeActors.Num() > 0)
	//{
	//	for (size_t i = 0; i < DetectionConditionItem.Amount; i++)
	//	{
	//		if (!InRangeActors.IsValidIndex(i)) break;
	//		TargetActors.Add(InRangeActors[i]);
	//	}
	//}

	return true;
}

void UDetectionSystemUtility::DetectionDebugDrawInvertedCone(
	const UObject* WorldContextObject,
	FVector OriginPointPos, FVector OriginPointReferenceDir, FRotator OriginPointRotator, float Radius, float Angle, FVector OriginPointPosOffset,
	int32 NumSides, float LifeTime, float Thickness)
{
	//探测中心点偏移
	if (OriginPointPosOffset != FVector::ZeroVector)
		OriginPointPos = OriginPointPos + OriginPointRotator.Quaternion() * OriginPointPosOffset;

	const FVector endPoint = OriginPointPos + OriginPointReferenceDir.GetSafeNormal() * Radius;
	//绘制探测轴心线
	UKismetSystemLibrary::DrawDebugArrow(WorldContextObject, OriginPointPos, endPoint, Thickness * 20.f, FLinearColor::Red, LifeTime, Thickness);

	float drawAngle = Angle / 2.f;
	//绘制圆锥范围
	UKismetSystemLibrary::DrawDebugConeInDegrees(WorldContextObject, OriginPointPos,
		OriginPointReferenceDir, Radius, drawAngle, drawAngle, NumSides,
		FLinearColor::Blue, LifeTime, Thickness);


	//绘制锥形底面
	TArray<FVector> drawConeVerts;
	drawConeVerts.AddUninitialized(10);
	drawConeVerts[2] = drawConeVerts[7] = endPoint;

	TArray<FVector> drawConeVertsTemp = UDetectionSystemUtility::GetConeVertsInDegrees(
		OriginPointPos, OriginPointReferenceDir, Radius, drawAngle, 4);
	drawConeVerts[0] = drawConeVertsTemp[0]; drawConeVerts[4] = drawConeVertsTemp[2];
	drawConeVerts[5] = drawConeVertsTemp[1]; drawConeVerts[9] = drawConeVertsTemp[3];

	drawConeVertsTemp = UDetectionSystemUtility::GetConeVertsInDegrees(
		OriginPointPos, OriginPointReferenceDir, Radius, drawAngle / 2.f, 4);
	drawConeVerts[1] = drawConeVertsTemp[0]; drawConeVerts[3] = drawConeVertsTemp[2];
	drawConeVerts[6] = drawConeVertsTemp[1]; drawConeVerts[8] = drawConeVertsTemp[3];

	for (size_t i = 0; i < 4; i++)
	{
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, drawConeVerts[i], drawConeVerts[i + 1], FLinearColor::Red, LifeTime, Thickness);
	}

	for (size_t i = 5; i < 9; i++)
	{
		UKismetSystemLibrary::DrawDebugLine(WorldContextObject, drawConeVerts[i], drawConeVerts[i + 1], FLinearColor::Red, LifeTime, Thickness);
	}
}

TArray<FVector> UDetectionSystemUtility::GetConeVertsInDegrees(FVector const& Origin, FVector const& Direction, float Length, const float Angle, int32 NumSides)
{
	NumSides = FMath::Max(NumSides, 4);

	float radians = FMath::DegreesToRadians(Angle);

	const float Angle1 = FMath::Clamp<float>(radians, (float)KINDA_SMALL_NUMBER, (float)(PI - KINDA_SMALL_NUMBER));
	const float Angle2 = FMath::Clamp<float>(radians, (float)KINDA_SMALL_NUMBER, (float)(PI - KINDA_SMALL_NUMBER));

	const float SinX_2 = FMath::Sin(0.5f * Angle1);
	const float SinY_2 = FMath::Sin(0.5f * Angle2);

	const float SinSqX_2 = SinX_2 * SinX_2;
	const float SinSqY_2 = SinY_2 * SinY_2;

	const float TanX_2 = FMath::Tan(0.5f * Angle1);
	const float TanY_2 = FMath::Tan(0.5f * Angle2);

	TArray<FVector> ConeVerts;
	ConeVerts.AddUninitialized(NumSides);

	for (size_t i = 0; i < NumSides; i++)
	{
		const float Fraction = (float)i / (float)(NumSides);
		const float Thi = 2.f * PI * Fraction;
		const float Phi = FMath::Atan2(FMath::Sin(Thi) * SinY_2, FMath::Cos(Thi) * SinX_2);
		const float SinPhi = FMath::Sin(Phi);
		const float CosPhi = FMath::Cos(Phi);
		const float SinSqPhi = SinPhi * SinPhi;
		const float CosSqPhi = CosPhi * CosPhi;

		const float RSq = SinSqX_2 * SinSqY_2 / (SinSqX_2 * SinSqPhi + SinSqY_2 * CosSqPhi);
		const float R = FMath::Sqrt(RSq);
		const float Sqr = FMath::Sqrt(1 - RSq);
		const float Alpha = R * CosPhi;
		const float Beta = R * SinPhi;

		ConeVerts[i].X = (1 - 2 * RSq);
		ConeVerts[i].Y = 2 * Sqr * Alpha;
		ConeVerts[i].Z = 2 * Sqr * Beta;
	}

	// Calculate transform for cone.
	FVector YAxis, ZAxis;
	FVector DirectionNorm = Direction.GetSafeNormal();
	DirectionNorm.FindBestAxisVectors(YAxis, ZAxis);
	const FMatrix ConeToWorld = FScaleMatrix(FVector(Length)) * FMatrix(DirectionNorm, YAxis, ZAxis, Origin);

	for (size_t i = 0; i < NumSides; i++)
	{
		ConeVerts[i] = ConeToWorld.TransformPosition(ConeVerts[i]);
	}

	return ConeVerts;
}

bool UDetectionSystemUtility::EqualsDetectionObjectData(const FDetectionObjectData& MyDataReferenceA, const FDetectionObjectData& MyDataReferenceB)
{
	return MyDataReferenceA.DetectionContainerID == MyDataReferenceB.DetectionContainerID
		&& MyDataReferenceA.Actor == MyDataReferenceB.Actor;
}
