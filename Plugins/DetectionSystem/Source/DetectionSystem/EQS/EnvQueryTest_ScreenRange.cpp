// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_ScreenRange.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Item.h"
#include <Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h>
#include <Runtime/Engine/Public/DrawDebugHelpers.h>
#include "Kismet/GameplayStatics.h"
#include "DetectionSystemWorldSubsystem.h"
#include "DetectionSystem/Widget/DetectionSystemDebugHUD.h"

UEnvQueryTest_ScreenRange::UEnvQueryTest_ScreenRange(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//必须添加此行 否则Test无法工作
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();

	TestMode = EEnvTestScreenRangeMode::ScreenPosition;
	DistanceTo = UEnvQueryContext_Querier::StaticClass();
	ScoreWeight = FVector2D(1.f, 1.f);
}

void UEnvQueryTest_ScreenRange::RunTest(FEnvQueryInstance& QueryInstance) const
{
	bool isServer = UKismetSystemLibrary::IsServer(this);

	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr) return;

	UWorld* World = GEngine->GetWorldFromContextObject(QueryOwner, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr) return;

	//获取游戏窗口
	UGameViewportClient* gameView = World->GetGameViewport();
	if (gameView == nullptr) return;
	

	//----获取基础数据----//

	//获取筛选设定的最高和最低分
	FloatValueMin.BindData(QueryOwner, QueryInstance.QueryID);
	float MinThresholdValue = FloatValueMin.GetValue();
	FloatValueMax.BindData(QueryOwner, QueryInstance.QueryID);
	float MaxThresholdValue = FloatValueMax.GetValue();

	//获取屏幕锚点位置数据
	float anchorX = Anchor.X;
	float anchorY = Anchor.Y;
	anchorX = FMath::Clamp(anchorX, 0.f, 1.f);
	anchorY = FMath::Clamp(anchorY, 0.f, 1.f);
	FVector2D anchor = FVector2D(anchorX, anchorY);

	FVector2D gameViewportSize = FVector2D(gameView->Viewport->GetSizeXY());//屏幕尺寸
	FVector2D originPos = gameViewportSize * anchor;//原点屏幕位置

	//原点位置偏移
	if (!PositionParam.IsZero())
	{
		FVector2D originPosOffset;
		switch (TestMode)
		{
		case EEnvTestScreenRangeMode::ScreenPosition:
			originPosOffset = PositionParam;
			break;
		case EEnvTestScreenRangeMode::ScerrnPositionPercent:
			originPosOffset = gameViewportSize * PositionParam;
			break;
		default:
			originPosOffset = PositionParam;
			break;
		}
		originPos = originPos + originPosOffset;
	}

	//获取限制距离(像素距离)，屏幕对角线*LimitDistanceParam
	float LimitDistance = -1;
	if (LimitDistanceParam > 0)
		LimitDistance = FMath::Sqrt(FMath::Square(gameViewportSize.X) + FMath::Square(gameViewportSize.Y)) * (LimitDistanceParam > 1 ? 1.f : LimitDistanceParam);
	//距离原点的最远点
	FVector2D farthestPoint =
		FVector2D
		(
			anchor.X < 0.5f ? gameViewportSize.X : 0.f,
			anchor.Y < 0.5f ? gameViewportSize.Y : 0.f
		);
	//获取距离原点的最远距离
	float disMax = FVector2D::Distance(farthestPoint, originPos);
	if (LimitDistance > 0 && disMax > LimitDistance)
		disMax = LimitDistance;

	//获取玩家相机到屏幕探测原点方向
	APlayerController* playerController = UGameplayStatics::GetPlayerController(World, 0);//获取玩家控制器
	FVector originPosWorldLocation; FVector originPosWorldDir;
	if (playerController)
		playerController->DeprojectScreenPositionToWorld(originPos.X, originPos.Y, originPosWorldLocation, originPosWorldDir);

	UDetectionSystemWorldSubsystem* detectionSystemWorldSubsystem = nullptr;
	GetDetectionSystemWorldSubsystem(detectionSystemWorldSubsystem);
	
#if !UE_BUILD_SHIPPING

	TArray<FDrawDebugBoxInfo> drawDebugBoxInfos;
	
	ADetectionSystemDebugHUD* detectionSystemDebugHUD = nullptr;
	if (detectionSystemWorldSubsystem != nullptr)
	{
		if (bDebugDraw)
		{
			detectionSystemDebugHUD = detectionSystemWorldSubsystem->GetDetectionSystemDebugHUD();
			if (!isServer && detectionSystemDebugHUD && UKismetSystemLibrary::IsValid(detectionSystemDebugHUD))
			{
				detectionSystemDebugHUD->SetDrawEQTScreenRange_Clear();
			}
		}
	}

	//Debug绘制探测范围
	if (bDebugDraw)
	{
		if (!isServer && detectionSystemDebugHUD && UKismetSystemLibrary::IsValid(detectionSystemDebugHUD))
		{
			//绘制屏幕范围
			detectionSystemDebugHUD->SetDrawEQTScreenRange(originPos, LimitDistance);
		}

		//绘制距离范围
		if (bOpenDistanceTest)
		{
			TArray<FVector> ContextLocations;
			if (QueryInstance.PrepareContext(DistanceTo, ContextLocations))
			{
				DrawDebugSphere(GetWorld(), ContextLocations[0], DistanceRange, 24, FColor::Blue, false, 0.1f);
			}
		}
	}

	//打印目标信息
	FString PrintDebugScoreStr;
	if (bDebugPrint)
	{
		PrintDebugScoreStr = "UEnvQueryTest_ScreenRange  ---  PrintDebugScoreStr : ";
	}
#endif

	//----对所有目标进行测试----//
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());//获取目标Actor
		if (bIgnoreSelfPawn && playerController && playerController->GetPawn() == ItemActor) continue;//排除自身控制Pawn

		FBox itemBox = GetBox(ItemActor);

		//----距离测试----//
		bool pass_Dis = true;
		float score_Dis = 1.f;
		if (bOpenDistanceTest)
		{
			TArray<FVector> ContextLocations;
			if (QueryInstance.PrepareContext(DistanceTo, ContextLocations))
			{
				//Box距离最近的点来计算距离
				float disWorld = FVector::Distance(ContextLocations[0], itemBox.GetClosestPointTo(ContextLocations[0]));
				score_Dis = (DistanceRange - disWorld) / DistanceRange;

				pass_Dis = score_Dis > 0;

#if !UE_BUILD_SHIPPING
				//Debug绘制目标点
				if (bDebugDraw)
				{
					if (score_Dis >= -0.2)//绘制一定范围内的目标点，包括不在有效范围内的
					{
						DrawDebugLine(GetWorld(), ContextLocations[0], itemBox.GetClosestPointTo(ContextLocations[0]), pass_Dis ? FColor::Green : FColor::Red, false, 0.1f, 0.f, 0.1f);

						drawDebugBoxInfos.Add(FDrawDebugBoxInfo(ItemActor, itemBox.GetCenter(), itemBox.GetExtent(), FColor::White));
					}
				}
#endif
			}
		}

		if (!pass_Dis) continue;//未通过距离测试

		//----获取目标世界坐标位置----//
		FVector itemLocation;
		bool getItemLocationSucceed = false;

		//获取包围盒上距离相机前方向量最近的点
		bool haveitemBox; FVector itemBoxCenter; bool isIntersection;
		FVector cameraForwardLineClosestPoint;
		if (bUseBoundingBoxClosestPoint && playerController && ItemActor != nullptr && UKismetSystemLibrary::IsValid(ItemActor))
		{
			haveitemBox = true;

			
			itemBoxCenter = itemBox.GetCenter();

			//获取包围盒上距离探测原点方向射线距离最近的点，作为目标点位置
			itemLocation = GetBoxClosestPointToLine2(itemBox, originPosWorldLocation, originPosWorldDir, isIntersection);
			getItemLocationSucceed = true;
		}

		//获取采样点提供的位置
		if (!getItemLocationSucceed)
		{
			itemLocation = QueryInstance.ItemTypeVectorCDO ?
				QueryInstance.ItemTypeVectorCDO->GetItemLocation(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset) :
				FVector::ZeroVector;
			getItemLocationSucceed = true;
		}

		//----屏幕范围测试----//
		bool bIsOnScreen;
		FVector2D itemPos = GetScreenPositionByWorldLocation(GetWorld(), playerController, itemLocation, bIsOnScreen);
		bool pass_ScreenRange = true;
		float score_ScreenRange = 1.f;
		if (IgnoreOutofScreen && !bIsOnScreen)
			pass_ScreenRange = false;
		else
		{
			float dis = FVector2D::Distance(originPos, itemPos);
			score_ScreenRange = (disMax - dis) / disMax;
			pass_ScreenRange = score_ScreenRange > 0;
		}

#if !UE_BUILD_SHIPPING
		//Debug绘制目标点
		if (bDebugDraw)
		{
			if (!isServer && detectionSystemDebugHUD && UKismetSystemLibrary::IsValid(detectionSystemDebugHUD))
				detectionSystemDebugHUD->SetDrawEQTScreenRange_ItemPoint(itemPos, pass_ScreenRange ? FLinearColor::Green : FLinearColor::Red);

			drawDebugBoxInfos[drawDebugBoxInfos.Num() - 1] = FDrawDebugBoxInfo(ItemActor, itemBox.GetCenter(), itemBox.GetExtent(), FColor::Blue);
		}
#endif

		if(!pass_ScreenRange) continue;//未通过屏幕范围测试


		//----计算最后得分----//
		float score;
		if (bOpenDistanceTest)
		{
			//根据分数权重去最终分数
			float score_ScreenRange_Weight = ScoreWeight.X >= 1 ? ScoreWeight.X : 1;
			float score_Dis_Weight = ScoreWeight.Y >= 1 ? ScoreWeight.Y : 1;
			score = (score_ScreenRange_Weight * score_ScreenRange + score_Dis_Weight * score_Dis) / (score_ScreenRange_Weight + score_Dis_Weight);
		}
		else
			score = score_ScreenRange;
		

		//----确认分数是否被筛除----//
		bool isFilter = TestPurpose == EEnvTestPurpose::Type::Filter || TestPurpose == EEnvTestPurpose::Type::FilterAndScore;
		bool isInFilterRange = true;
		if (isFilter)
		{
			switch (FilterType)
			{
				case EEnvTestFilterType::Type::Range:
					isInFilterRange = score > MinThresholdValue && score < MaxThresholdValue;
					break;
				case EEnvTestFilterType::Type::Minimum:
					isInFilterRange = score > MinThresholdValue;
					break;
				case EEnvTestFilterType::Type::Maximum:
					isInFilterRange = score < MaxThresholdValue;
					break;
			}
		}

#if !UE_BUILD_SHIPPING
		//Debug绘制分数
		if (bDebugDraw)
		{
			if(!isServer && detectionSystemDebugHUD && UKismetSystemLibrary::IsValid(detectionSystemDebugHUD))
				detectionSystemDebugHUD->SetDrawEQTScreenRange_Score(FString::SanitizeFloat(score), itemLocation, isInFilterRange ? FColor::Green : FColor::Red);
		}
#endif

		//----进行遮挡检查----//
		if (isInFilterRange && OcclusionExamineMode != EEnvTestScreenRangeOcclusionExamineMode::None)
		{
			//traceEnd默认为itemLocation，为包围盒上的点，我们能保证Tarce对准ItemBox，但不能保证射线一定能击打到Item目标
			//击打到目标或者没有击打到任何物品都算通过测试

			bool bHit = false;
			FHitResult hitResaule;
			AActor* hitTarget = nullptr;
			bool isOcclusion = false;
			bool succeedHit = false;
			FVector traceEnd = itemLocation;
			FCollisionQueryParams TraceParams(FName(TEXT("ScreenRangeTestTrace")), false, bIgnoreSelfPawn ? playerController->GetPawn() : nullptr);

			if (detectionSystemWorldSubsystem && OcclusionExamineMode == EEnvTestScreenRangeOcclusionExamineMode::ForecastTrace)
			{
				//获取上次射线信息
				FOcclusionExamineTraceInfo forecastTraceInfo;
				if (!detectionSystemWorldSubsystem->GetForecastTracePos(ItemActor, forecastTraceInfo))
				{
					//重置数据
					forecastTraceInfo.ForecastTraceScreenPos = originPos;
				}

				//上次射线方向是成功的，优先使用
				if (forecastTraceInfo.SucceedHit)
				{
					//保证在屏幕范围内
					FVector2D traceEndScreenPos;
					playerController->ProjectWorldLocationToScreen(forecastTraceInfo.EndLocation, traceEndScreenPos);
					if (FVector2D::Distance(originPos, traceEndScreenPos) > disMax)
					{
						traceEndScreenPos = originPos + (traceEndScreenPos - originPos).GetSafeNormal() * disMax;
						FVector traceEndLocation; FVector traceEndLocationDir;
						playerController->DeprojectScreenPositionToWorld(traceEndScreenPos.X, traceEndScreenPos.Y, traceEndLocation, traceEndLocationDir);
						forecastTraceInfo.EndLocation = traceEndLocation + traceEndLocationDir * (FVector::Distance(originPosWorldLocation, itemLocation) * 2.f);
					}

					traceEnd = forecastTraceInfo.EndLocation;
				}
				
				bHit = World->LineTraceSingleByChannel(
					hitResaule,
					originPosWorldLocation,
					traceEnd,
					OcclusionExamineTraceChannel.GetValue(), TraceParams);

				hitTarget = hitResaule.GetActor();
				isOcclusion = bHit && hitTarget != ItemActor;//被其他物体阻挡
				succeedHit = bHit && hitTarget == ItemActor;//成功击打到自身

				//射线被其他物体阻挡了
				if (isOcclusion)
				{
					//预测射线符合角色探头直觉，射线会往鼠标最后移动方向倾斜

					//获取鼠标移动
					float MouseDeltaX; float MouseDeltaY;
					playerController->GetInputMouseDelta(MouseDeltaX, MouseDeltaY);
					FVector2D MouseDeltaDir = FVector2D(MouseDeltaX, -MouseDeltaY);

					forecastTraceInfo.ForecastTraceScreenPos = forecastTraceInfo.ForecastTraceScreenPos + MouseDeltaDir.GetSafeNormal() * disMax * OcclusionExamineForecastTraceMoveRate;
					if (FVector2D::Distance(originPos, forecastTraceInfo.ForecastTraceScreenPos) > disMax)
					{
						forecastTraceInfo.ForecastTraceScreenPos = originPos + (forecastTraceInfo.ForecastTraceScreenPos - originPos).GetSafeNormal() * disMax;
					}

					//获取射线倾向方向世界坐标
					FVector traceTendencyPosWorldLocation; FVector traceTendencyPosWorldDir;
					playerController->DeprojectScreenPositionToWorld(
						forecastTraceInfo.ForecastTraceScreenPos.X,
						forecastTraceInfo.ForecastTraceScreenPos.Y,
						traceTendencyPosWorldLocation, traceTendencyPosWorldDir);

					bool traceTendencyIsIntersection;
					traceEnd = GetBoxClosestPointToLine2(itemBox, traceTendencyPosWorldLocation, traceTendencyPosWorldDir, traceTendencyIsIntersection);

					bHit = World->LineTraceSingleByChannel(
						hitResaule,
						originPosWorldLocation,
						traceEnd,
						OcclusionExamineTraceChannel.GetValue(), TraceParams);

					hitTarget = hitResaule.GetActor();
					isOcclusion = bHit && hitTarget != ItemActor;//被其他物体阻挡
					succeedHit = bHit && hitTarget == ItemActor;//成功击打到自身

#if !UE_BUILD_SHIPPING
					//Debug绘制预测射线屏幕位置
					if (bDebugDraw)
					{
						if (!isServer && detectionSystemDebugHUD && UKismetSystemLibrary::IsValid(detectionSystemDebugHUD))
							detectionSystemDebugHUD->SetDrawEQTScreenRange_ItemPoint(forecastTraceInfo.ForecastTraceScreenPos, FLinearColor::Yellow);
					}
#endif
				}

				//成功击打到自身时，记录信息
				if (succeedHit)
				{
					forecastTraceInfo.SucceedHit = succeedHit;
					forecastTraceInfo.EndLocation = traceEnd;
				}

				//更新射线信息
				detectionSystemWorldSubsystem->UpdateForecastTracePos(ItemActor, forecastTraceInfo);
			}
			else /*if(OcclusionExamineMode == EEnvTestScreenRangeOcclusionExamineMode::SingleTrace)*/
			{
				//默认从原点往目标点发射射线进行碰撞检测
				bHit = World->LineTraceSingleByChannel(
					hitResaule,
					originPosWorldLocation,
					traceEnd,
					OcclusionExamineTraceChannel.GetValue(), TraceParams);

				hitTarget = hitResaule.GetActor();
				isOcclusion = bHit && hitTarget != ItemActor;//被其他物体阻挡
				succeedHit = bHit && hitTarget == ItemActor;//成功击打到自身
			}

#if !UE_BUILD_SHIPPING
			if (bDebugPrint && hitTarget != nullptr)
			{
				//打印射线击打到的目标信息
				UKismetSystemLibrary::PrintString(GetWorld(), "OcclusionExamine  ---  hitActor : " + AActor::GetDebugName(hitTarget), true, true, FLinearColor::Blue, 0.1f);
			}

			if (bDebugDraw)
			{
				FColor traceColor = FColor::White;
				if (bHit)
				{
					traceColor = isOcclusion ? FColor::Red : FColor::Green;

					FBox hitTargetitemBox = GetBox(hitTarget);
					FDrawDebugBoxInfo drawDebugBoxInfo = FDrawDebugBoxInfo(hitTarget, hitTargetitemBox.GetCenter(), hitTargetitemBox.GetExtent(), traceColor);
					int index = drawDebugBoxInfos.Num() - 1;
					if (isOcclusion)
					{
						for (int i = 0; i < drawDebugBoxInfos.Num(); i++)
						{
							//被其他Item阻挡时，绘制黄色碰撞盒
							if (drawDebugBoxInfos[i].TargetActor == hitTarget)
							{
								index = i;
								drawDebugBoxInfo.Color = FColor::Yellow;
							}
						}
					}
					drawDebugBoxInfos[index] = drawDebugBoxInfo;
				}

				//绘制射线
				//DrawDebugLine(GetWorld(), originPosWorldLocation, traceEnd, traceColor, false, 0.1f, 0, 0.18f);
				DrawDebugDirectionalArrow(GetWorld(), originPosWorldLocation, traceEnd, 2.f, traceColor, false, 0.1f, 0, 0.007f);
			}
#endif

			if (isOcclusion)
			{
				score = -1.f;
				continue;//未通过阻挡检测测试
			}
		}

		//打分
		It.SetScore(TestPurpose, FilterType, score, MinThresholdValue, MaxThresholdValue);

#if !UE_BUILD_SHIPPING
		if (bDebugPrint)
		{
			//此目标的相关信息
			PrintDebugScoreStr = PrintDebugScoreStr + (ItemActor != nullptr ? AActor::GetDebugName(ItemActor) : FString::FromInt(It.GetIndex())) + " : " + FString::SanitizeFloat(score) + " | ";
		}
#endif
	}

	if(detectionSystemWorldSubsystem)
		detectionSystemWorldSubsystem->OnUEnvQueryTestScreenRangeOver();

#if !UE_BUILD_SHIPPING
	if (bDebugDraw)
	{
		for (int i = 0; i < drawDebugBoxInfos.Num(); i++)
		{
			FDrawDebugBoxInfo info = drawDebugBoxInfos[i];

			DrawDebugBox(GetWorld(), info.Center, info.Extent, info.Color, false, 0.1f);
		}
	}

	if (bDebugPrint)
	{
		//打印所有有效目标的分数信息
		UKismetSystemLibrary::PrintString(GetWorld(), PrintDebugScoreStr, true, true, FLinearColor::Blue, 0.1f);
	}
#endif
}

FText UEnvQueryTest_ScreenRange::GetDescriptionTitle() const
{
	return FText::FromString(FString::Printf(TEXT("%s: Anchor %s and Position %s"),
		*Super::GetDescriptionTitle().ToString(), *Anchor.ToString(), *PositionParam.ToString()));
}

FText UEnvQueryTest_ScreenRange::GetDescriptionDetails() const
{
	return FText();
}

FVector2D UEnvQueryTest_ScreenRange::GetScreenPositionByWorldLocation(UWorld* World, APlayerController* PlayerController, FVector InLocation, bool& bIsOnScreen) const
{
	if (World == nullptr) return FVector2D();
	if (PlayerController == nullptr) return FVector2D();

	FVector2D ScreenPosition = FVector2D();
	const FVector2D ViewportSize = FVector2D(World->GetGameViewport()->Viewport->GetSizeXY());//获取屏幕尺寸

	FVector CameraLoc;
	FRotator CameraRot;

	PlayerController->GetPlayerViewPoint(CameraLoc, CameraRot);

	const FVector CameraToLoc = InLocation - CameraLoc;
	FVector Forward = CameraRot.Vector();
	FVector Offset = CameraToLoc.GetSafeNormal();

	float DotProduct = FVector::DotProduct(Forward, Offset);
	bool bLocationIsBehindCamera = (DotProduct < 0);

	PlayerController->ProjectWorldLocationToScreen(InLocation, ScreenPosition);

	//确认目标位置是否在屏幕上
	if (!bLocationIsBehindCamera
		&& (ScreenPosition.X >= 0.f && ScreenPosition.X <= ViewportSize.X)
		&& (ScreenPosition.Y >= 0.f && ScreenPosition.Y <= ViewportSize.Y))
	{
		bIsOnScreen = true;
	}
	else
		bIsOnScreen = false;

	return ScreenPosition;
}

FBox UEnvQueryTest_ScreenRange::GetBox(AActor* Actor) const
{
	FBox box(ForceInit);

	if (Actor == nullptr || !UKismetSystemLibrary::IsValid(Actor)) return box;

	switch (GetBoxMode)
	{
	case EEnvTestScreenRangeGetBoxMode::Primitive:
		Actor->ForEachComponent<UPrimitiveComponent>(false, [&](const UPrimitiveComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});
		break;
	case EEnvTestScreenRangeGetBoxMode::Mesh:
		Actor->ForEachComponent<UMeshComponent>(false, [&](const UMeshComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});
		break;
	case EEnvTestScreenRangeGetBoxMode::StaticMesh:
		Actor->ForEachComponent<UStaticMeshComponent>(false, [&](const UStaticMeshComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});
		break;
	case EEnvTestScreenRangeGetBoxMode::SkinnedMesh:
		Actor->ForEachComponent<USkinnedMeshComponent>(false, [&](const USkinnedMeshComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});
		break;
	case EEnvTestScreenRangeGetBoxMode::StaticMeshAndSkinnedMesh:
		Actor->ForEachComponent<UStaticMeshComponent>(false, [&](const UStaticMeshComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});

		Actor->ForEachComponent<USkeletalMeshComponent>(false, [&](const USkeletalMeshComponent* Comp)
			{
				GetBox_BoxAdd(Comp, box);
			});
		break;
	default:
		break;
	}

	if (box.GetSize().IsZero())
	{
		//通过Actor获取包围盒
		//当Actor带有Trigger功能没有Block自身的大于模型的Collider时，可能和预期不符
		box = Actor->GetComponentsBoundingBox();
	}

	return box;
}

void UEnvQueryTest_ScreenRange::GetBox_BoxAdd(const UPrimitiveComponent* Component, FBox& Box)  const
{
	if (Component->IsRegistered() && Component->IsCollisionEnabled() && Component->IsVisible())
	{
		Box += Component->Bounds.GetBox();
	}
}

FVector UEnvQueryTest_ScreenRange::ClosestPointOnInfiniteLine(const FVector& LineStart, const FVector& Dir, const FVector& Point) const
{
	const float A = (LineStart - Point) | Dir;
	const float B = (-Dir).SizeSquared();
	if (B < SMALL_NUMBER)
	{
		return LineStart;
	}
	const float T = -A / B;

	const FVector ClosestPoint = LineStart + (T * Dir);
	return ClosestPoint;
}

FVector UEnvQueryTest_ScreenRange::GetBoxClosestPointToLine1(const FBox& Box, const FVector& Start, const FVector& Dir) const
{
	//粗略计算，当目标和Dir相同方向的长度过长时会导致实际可探测体积小于Box
	FVector cameraForwardLineClosestPoint = ClosestPointOnInfiniteLine(Start, Dir, Box.GetCenter());
	return Box.GetClosestPointTo(cameraForwardLineClosestPoint);
}

FVector UEnvQueryTest_ScreenRange::GetBoxClosestPointToLine2(const FBox& Box, const FVector& Start, const FVector& Dir, bool& IsIntersection) const
{
	//计算线段结束点
	float length = (Start - Box.GetCenter()).Size() + Box.GetSize().Size();
	FVector End = Start + Dir * length;

	//线段与Box交叉时，返回交叉点
	FVector HitLocation; FVector HitNormal; float HitTime;
	IsIntersection = FMath::LineExtentBoxIntersection(Box, Start, End, FVector::ZeroVector, HitLocation, HitNormal, HitTime);
	if (IsIntersection)
		return HitLocation;
	
	//Box八个角点
	FVector point1 = Box.Min;
	FVector point7 = Box.Max;
	FVector point2 = FVector(point7.X, point1.Y, point1.Z);
	FVector point3 = FVector(point7.X, point7.Y, point1.Z);
	FVector point4 = FVector(point1.X, point7.Y, point1.Z);
	FVector point5 = FVector(point1.X, point1.Y, point7.Z);
	FVector point6 = FVector(point7.X, point1.Y, point7.Z);
	FVector point8 = FVector(point1.X, point7.Y, point7.Z);

	//获取可以忽略的角点。距离自身最远的角点相连的三条边可以忽略
	float disMax = FLT_MIN;
	int ignorePoint = -1;
	for (int i = 0; i < 8; i++)
	{
		FVector pointTemp;
		switch (i)
		{
		case 0:
			pointTemp = point1;
			break;
		case 1:
			pointTemp = point2;
			break;
		case 2:
			pointTemp = point3;
			break;
		case 3:
			pointTemp = point4;
			break;
		case 4:
			pointTemp = point5;
			break;
		case 5:
			pointTemp = point6;
			break;
		case 6:
			pointTemp = point7;
			break;
		case 7:
			pointTemp = point8;
			break;
		}

		float pointDis = FVector::Distance(Start, pointTemp);
		if (pointDis > disMax)
		{
			disMax = pointDis;
			ignorePoint = i + 1;
		}
	}

	//获取Box的边上距离线段最近的点
	FVector closestPointOnBox;
	float twoPointDis = FLT_MAX;
	for (int i = 0; i < 12; i++)
	{
		FVector p1; FVector p2;
		switch (i)
		{
		case 0:
			if (ignorePoint == 1 || ignorePoint == 2) continue;
			p1 = point1;
			p2 = point2;
			break;
		case 1:
			if (ignorePoint == 2 || ignorePoint == 3) continue;
			p1 = point2;
			p2 = point3;
			break;
		case 2:
			if (ignorePoint == 3 || ignorePoint == 4) continue;
			p1 = point3;
			p2 = point4;
			break;
		case 3:
			if (ignorePoint == 4 || ignorePoint == 1) continue;
			p1 = point4;
			p2 = point1;
			break;
		case 4:
			if (ignorePoint == 5 || ignorePoint == 6) continue;
			p1 = point5;
			p2 = point6;
			break;
		case 5:
			if (ignorePoint == 6 || ignorePoint == 7) continue;
			p1 = point6;
			p2 = point7;
			break;
		case 6:
			if (ignorePoint == 7 || ignorePoint == 8) continue;
			p1 = point7;
			p2 = point8;
			break;
		case 7:
			if (ignorePoint == 8 || ignorePoint == 5) continue;
			p1 = point8;
			p2 = point5;
			break;
		case 8:
			if (ignorePoint == 1 || ignorePoint == 5) continue;
			p1 = point1;
			p2 = point5;
			break;
		case 9:
			if (ignorePoint == 2 || ignorePoint == 6) continue;
			p1 = point2;
			p2 = point6;
			break;
		case 10:
			if (ignorePoint == 3 || ignorePoint == 7) continue;
			p1 = point3;
			p2 = point7;
			break;
		case 11:
			if (ignorePoint == 4 || ignorePoint == 8) continue;
			p1 = point4;
			p2 = point8;
			break;
		}

		FVector OutP1; FVector OutP2;
		FMath::SegmentDistToSegment(Start, End, p1, p2, OutP1, OutP2);
		float dis = FVector::Distance(OutP1, OutP2);
		if (dis < twoPointDis)
		{
			closestPointOnBox = OutP2;
			twoPointDis = dis;
		}
	}

	return closestPointOnBox;
}

bool UEnvQueryTest_ScreenRange::GetDetectionSystemWorldSubsystem(UDetectionSystemWorldSubsystem*& OutSubsystem) const
{
	OutSubsystem = nullptr;

	UWorld* world = GetWorld();
	if (world == nullptr) return false;

	OutSubsystem = world->GetSubsystem<UDetectionSystemWorldSubsystem>();

	return OutSubsystem != nullptr;
}
