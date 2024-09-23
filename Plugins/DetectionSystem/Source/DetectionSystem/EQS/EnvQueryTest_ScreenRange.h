// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_ScreenRange.generated.h"

class ADetectionSystemDebugHUD;

/*阻挡检测模式*/
UENUM(BlueprintType)
enum class EEnvTestScreenRangeOcclusionExamineMode : uint8
{
	/*不进行阻挡检查，如果目标不会被任何物体阻挡，不需要进行阻挡检查以节省开销。*/
	None,

	/*单射线检测*/
	SingleTrace,

	/*预测射线，在目标经常被障碍物遮挡时此模式将获得更好的体验。但会有更多的运算和内存开销。*/
	ForecastTrace,
};

/*获取包围盒模式*/
UENUM(BlueprintType)
enum class EEnvTestScreenRangeGetBoxMode : uint8
{
	/*传统组件，几乎包括所有类型的组件*/
	Primitive,

	/*网格组件，主要指在3D场景中渲染可见的组件。除了Static和Skinned，还包括WidgetComponent等组件*/
	Mesh,

	/*静态网格组件，一些摆放在场景中的静态对象*/
	StaticMesh,

	/*蒙皮网格组件*/
	SkinnedMesh,

	/*蒙皮网格和静态网格*/
	StaticMeshAndSkinnedMesh
};

UENUM(BlueprintType)
enum class EEnvTestScreenRangeMode : uint8
{
	/*屏幕位置设置参数PositionParam为像素尺寸*/
	ScreenPosition,

	/*屏幕位置设置参数PositionParam为百分比 0-1*/
	ScerrnPositionPercent,

	//ContextToScerrnPosition,
};

USTRUCT()
struct FDrawDebugBoxInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
		AActor* TargetActor;

	UPROPERTY()
		FVector Center;

	UPROPERTY()
		FVector Extent;

	UPROPERTY()
		FColor Color;

	FDrawDebugBoxInfo() {}

	FDrawDebugBoxInfo(AActor* InTargetActor, FVector InCenter, FVector InExtent, FColor InColor)
		: TargetActor(InTargetActor)
		, Center(InCenter)
		, Extent(InExtent)
		, Color(InColor)
	{
	}

	bool operator == (const FDrawDebugBoxInfo other) const
	{
		return TargetActor == other.TargetActor;
	}
};

/**
 * 屏幕范围测试
 * 确认目标世界位置在屏幕上的投射位置，是否在要求的范围内。
 * 带3D空间距离检测。如果希望空间距离分数和屏幕范围分数以自定义的权重取平均分，可以打开此测试的距离测试。
 */
UCLASS()
class DETECTIONSYSTEM_API UEnvQueryTest_ScreenRange : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
protected:

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		EEnvTestScreenRangeMode TestMode;

	/*获取目标包围盒模式*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		EEnvTestScreenRangeGetBoxMode GetBoxMode = EEnvTestScreenRangeGetBoxMode::StaticMeshAndSkinnedMesh;

	/**
	 * @brief 锚点，范围中心点，探测范围原点。 比例参数 左下角为(0,0)  输入0-1
	*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		FVector2D Anchor;

	/**
	 * @brief 屏幕坐标位置参数 根据TestMode不同 有不同的含义
	*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		FVector2D PositionParam;

	/**
	 * 距离限制 (比例参数 0-1 限制距离(像素距离)=游戏窗口对角线像素长度*限制距离参数)
	 * 超出此屏幕距离范围的目标为最低分
	 * 如果设置为<0 最远距离为原点到屏幕边缘的最远直线距离
	*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		float LimitDistanceParam = -1;

	/*忽略自己控制的Pawn*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		bool bIgnoreSelfPawn = true;

	/*使用BoundingBox距离中心点最近的点作为目标点*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		bool bUseBoundingBoxClosestPoint = true;


	/*阻挡检查*/
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		EEnvTestScreenRangeOcclusionExamineMode OcclusionExamineMode = EEnvTestScreenRangeOcclusionExamineMode::SingleTrace;
	
	/*
	* 阻挡检查射线通道
	* 此通道应当包含探测目标
	*/
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		TEnumAsByte<ECollisionChannel> OcclusionExamineTraceChannel = ECollisionChannel::ECC_WorldStatic;

	/*
	* 阻挡检查，预测射线模式时射线方向移动速率 (0-1)
	*/
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		float OcclusionExamineForecastTraceMoveRate = 0.3f;


	/*启用距离测试，世界坐标范围*/
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		bool bOpenDistanceTest = true;

	/*其他目标到此位置的距离*/
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		TSubclassOf<UEnvQueryContext> DistanceTo;

	/*距离范围*/
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		float DistanceRange = 150.f;


	/*
	* 分数权重(屏幕范围分权重，距离分权重)
	* 权重值必须大于等于1，小于此值时认定为1
	*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FVector2D ScoreWeight;


	/*绘制Debug信息*/
	UPROPERTY(EditDefaultsOnly, Category = Debug)
		bool bDebugDraw = false;

	/*打印Debug信息*/
	UPROPERTY(EditDefaultsOnly, Category = Debug)
		bool bDebugPrint = false;


	/*忽略在镜头外的*/
	UPROPERTY()
		bool IgnoreOutofScreen = true;


	/*获取世界坐标在屏幕中的坐标*/
	UFUNCTION()
		FVector2D GetScreenPositionByWorldLocation(UWorld* World, APlayerController* PlayerController, FVector InLocation, bool& bIsOnScreen) const;

	/*获取Actor包围盒，根据设定的GetBoxMode*/
	UFUNCTION()
		FBox GetBox(AActor* Actor) const;

	/*获取包围盒时，确认组件是否有效*/
	UFUNCTION()
		void GetBox_BoxAdd(const UPrimitiveComponent* Component, FBox& Box) const;

	/*获取直线上距离某个点最近的点*/
	UFUNCTION()
		FVector ClosestPointOnInfiniteLine(const FVector& LineStart, const FVector& Dir, const FVector& Point) const;

	/*获取Box上距离射线最近的点，粗略获取方法*/
	UFUNCTION()
		FVector GetBoxClosestPointToLine1(const FBox& Box, const FVector& Start, const FVector& Dir) const;

	/*获取Box上距离射线最近的点，准确获取方法*/
	UFUNCTION()
		FVector GetBoxClosestPointToLine2(const FBox& Box, const FVector& Start, const FVector& Dir, bool& IsIntersection) const;

	UFUNCTION()
		bool GetDetectionSystemWorldSubsystem(UDetectionSystemWorldSubsystem*& OutSubsystem) const;
};
