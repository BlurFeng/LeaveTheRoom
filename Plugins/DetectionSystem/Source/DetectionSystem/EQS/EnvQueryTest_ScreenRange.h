// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_ScreenRange.generated.h"

class ADetectionSystemDebugHUD;

/**
 * 屏幕空间测试。确认目标是否在要求的屏幕空间范围内。之后可以通过射线检测来确定是否被其他物体阻挡。
 * Screen space test. Check if the target is within the required screen space range. After that, raycasting can be used to determine if it is obstructed by other objects.
 * スクリーンスペーステスト。ターゲットが要求されたスクリーンスペース範囲内にあるか確認します。その後、レイキャストを使用して他のオブジェクトによって遮られているかどうかを判断できます。
 */
UENUM(BlueprintType)
enum class EEnvTestScreenRangeOcclusionExamineMode : uint8
{
	/**
	 * 不进行阻挡检查。项目不需要阻挡检测时，可节省射线的开销。
	 * No obstruction checks. When obstruction detection is not needed for the item, it can save the overhead of raycasting.
	 * 遮蔽チェックを行いません。アイテムに遮蔽検出が必要ない場合、レイキャスティングのオーバーヘッドを節約できます。
	 */
	None,

	SingleTrace,

	/**
	 * 预测射线。会有更多的运算和内存开销。在遮挡情况复杂时可开启以改善体验。比如通过墙上的一个小洞对准对面的目标时。
	 * Predictive raycasting. This will incur more computational and memory overhead. It can be enabled to improve the experience in complex occlusion scenarios, such as aiming at a target across the room through a small hole in the wall.
	 * 予測レイキャスティング。これにより、より多くの計算とメモリのオーバーヘッドが発生します。複雑な遮蔽シナリオで体験を改善するために有効にすることができます。例えば、壁の小さな穴を通して対面のターゲットを狙う場合などです。
	 */
	ForecastTrace,
};

/**
 * 获取探测目标的包围盒模式。
 * Get the bounding box mode of the detection target.
 * 検出ターゲットのバウンディングボックスモードを取得します。
 */
UENUM(BlueprintType)
enum class EEnvTestScreenRangeGetBoxMode : uint8
{
	/**
	 * 传统组件，几乎包括所有类型的组件。
	 * Traditional components, which include almost all types of components.
	 * 従来のコンポーネントで、ほぼすべてのタイプのコンポーネントが含まれています。
	 */
	Primitive,

	/**
	 * 网格组件，主要指在3D场景中渲染可见的组件。除了Static和Skinned，还包括WidgetComponent等组件。
	 * Mesh components, primarily referring to components that render visible elements in a 3D scene. In addition to Static and Skinned, it also includes components like WidgetComponent.
	 * メッシュコンポーネントは、主に3Dシーンで可視要素をレンダリングするコンポーネントを指します。StaticやSkinnedに加えて、WidgetComponentなどのコンポーネントも含まれています。
	 */
	Mesh,

	/**
	 * 静态网格组件，一些摆放在场景中的静态对象。
	 * Static mesh components, which are static objects placed in the scene.
	 * スタティックメッシュコンポーネントは、シーンに配置された静的オブジェクトです。
	 */
	StaticMesh,

	/**
	 * 蒙皮网格组件。角色等对象会使用。
	 * Skinned mesh components, used for objects such as characters.
	 * スキンメッシュコンポーネントは、キャラクターなどのオブジェクトに使用されます。
	 */
	SkinnedMesh,

	/**
	 * 蒙皮网格和静态网格。最常用的模式，包括一般的场景物体和角色等对象。
	 * Skinned mesh and static mesh. The most commonly used types, including general scene objects and characters.
	 * スキンメッシュとスタティックメッシュ。一般的なシーンオブジェクトやキャラクターなど、最も一般的に使用されるタイプです。
	 */
	StaticMeshAndSkinnedMesh
};

/**
 * 屏幕范围模式，决定如何去使用配置的参数。
 * Screen range mode, which determines how to use the configured parameters.
 * スクリーン範囲モードは、設定されたパラメータをどのように使用するかを決定します。
 */
UENUM(BlueprintType)
enum class EEnvTestScreenRangeMode : uint8
{
	/**
	 * 屏幕位置设置参数PositionParam为像素尺寸。
	 * The screen position setting parameter PositionParam is in pixel size.
	 * スクリーン位置設定パラメータPositionParamは、ピクセルサイズです。
	 */
	ScreenPosition,

	/**
	 * 屏幕位置设置参数PositionParam为百分比0-1。
	 * The screen position setting parameter PositionParam is a percentage from 0 to 1.
	 * スクリーン位置設定パラメータPositionParamは、0から1のパーセンテージです。
	 */
	ScerrnPositionPercent,

	//ContextToScerrnPosition,
};

/**
 * 用于绘制Debug时用的包围盒。
 * Bounding box used for drawing debug visuals.
 * デバッグ用の視覚表示を描画するためのバウンディングボックス。
 */
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
 * 屏幕空间范围测试。确认目标世界位置在屏幕上的投射位置，是否在要求的范围内。也能够进行3D空间距离检测，且可设置打分权重。
 * Screen space range test. Check if the projected position of the target's world position on the screen is within the required range. It can also perform 3D space distance detection and allows setting scoring weights.
 * スクリーンスペース範囲テスト。ターゲットのワールド位置がスクリーン上に投影された位置が要求された範囲内にあるか確認します。また、3D空間での距離検出を行うことができ、スコアの重み付けを設定できます。
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

	/**
	 * 获取目标包围盒模式
	 * Get the bounding box mode of the target.
	 * ターゲットのバウンディングボックスモードを取得します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		EEnvTestScreenRangeGetBoxMode GetBoxMode = EEnvTestScreenRangeGetBoxMode::StaticMeshAndSkinnedMesh;

	/**
	 * 锚点，范围中心点，探测范围原点。比例参数 左下角为(0,0)，输入0-1。
	 * Anchor point, range center point, detection range origin. Scale parameters, with the bottom-left corner as (0,0), input values from 0 to 1.
	 * アンカーポイント、範囲の中心点、検出範囲の原点。スケールパラメータは左下隅を(0,0)とし、0から1の値を入力します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		FVector2D Anchor;

	/**
	 * 屏幕坐标位置参数，根据TestMode不同，有不同的含义。
	 * Screen coordinate position parameter, which has different meanings depending on the TestMode.
	 * スクリーン座標位置パラメータは、TestModeに応じて異なる意味を持ちます。
	 */
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		FVector2D PositionParam;

	/**
	 * 距离限制 (比例参数0-1，限制距离(像素距离)等于游戏窗口对角线像素长度*限制距离参数)。超出此屏幕距离范围的目标为最低分。
	 * 如果设置为小于0，最远距离为原点到屏幕边缘的最远直线距离。
	 * Distance limit (scale parameter 0-1), where the restricted distance (pixel distance) is equal to the pixel length of the game window's diagonal multiplied by the distance limit parameter. Targets exceeding this screen distance range receive the lowest score.
	 * If set to less than 0, the maximum distance is the farthest straight-line distance from the origin to the edge of the screen.
	 * 距離制限（スケールパラメータ0-1）。制限距離（ピクセル距離）は、ゲームウィンドウの対角線のピクセル長さに制限距離パラメータを掛けたものと等しくなります。このスクリーン距離範囲を超えたターゲットは最低スコアを受け取ります。
	 * 0未満に設定すると、最遠距離は原点から画面の端までの最遠直線距離になります。
	*/
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		float LimitDistanceParam = -1;

	/**
	 * 忽略自己控制的Pawn。
	 * Ignore the player-controlled pawn.
	 * プレイヤーが操作するポーンを無視します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		bool bIgnoreSelfPawn = true;

	/**
	 * 使用BoundingBox距离中心点最近的点作为目标点，这在目标体积较大时，能对准目标体积边缘就获取目标。否则获取目标中心点作为目标点。
	 * Use the point on the BoundingBox that is closest to the center point as the target point. This allows for targeting the edge of a larger target volume. Otherwise, use the center point of the target as the target point.
	 * バウンディングボックスで中心点に最も近い点をターゲットポイントとして使用します。これにより、大きなターゲットボリュームの端を狙うことができます。それ以外の場合は、ターゲットの中心点をターゲットポイントとして使用します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = ScreenRange)
		bool bUseBoundingBoxClosestPoint = true;


	/**
	 * 阻挡检查。
	 * Obstruction check.
	 * 遮蔽チェック。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		EEnvTestScreenRangeOcclusionExamineMode OcclusionExamineMode = EEnvTestScreenRangeOcclusionExamineMode::SingleTrace;
	
	/*
	* 阻挡检查射线通道，此通道应当包含探测目标。
	* Obstruction check ray channel; this channel should include the detected target.
	* 遮蔽チェックのレイチャネル。このチャネルには検出されたターゲットが含まれている必要があります。
	*/
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		TEnumAsByte<ECollisionChannel> OcclusionExamineTraceChannel = ECollisionChannel::ECC_WorldStatic;

	/**
	 * 阻挡检查，预测射线模式时射线方向移动速率 (0-1)。
	 * Obstruction check, ray movement speed in predictive ray mode (0-1).
	 * 遮蔽チェック、予測レイモードでのレイの移動速度（0-1）。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
		float OcclusionExamineForecastTraceMoveRate = 0.3f;


	/**
	 * 启用距离测试，世界坐标范围。
	 * Enable distance test, world coordinate range.
	 * 距離テストを有効にし、ワールド座標範囲を設定します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		bool bOpenDistanceTest = true;

	/**
	 * 其他目标到此位置的距离。
	 * The distance from other targets to this position.
	 * 他のターゲットからこの位置までの距離。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		TSubclassOf<UEnvQueryContext> DistanceTo;

	/**
	 * 距离范围。
	 * Distance range.
	 * 距離範囲。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Distance)
		float DistanceRange = 150.f;


	/*
	* 分数权重(屏幕范围分权重，距离分权重)，权重值必须大于等于1，小于此值时认定为1。
	* Score weights (screen range weight, distance weight); the weight value must be greater than or equal to 1. If it is less than this value, it is considered as 1.
	* スコアの重み（スクリーン範囲の重み、距離の重み）。重み値は1以上でなければならず、これ未満の場合は1と見なされます。
	*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
		FVector2D ScoreWeight;


	/**
	 * 绘制Debug信息。
	 * Draw debug information.
	 * デバッグ情報を描画します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Debug)
		bool bDebugDraw = false;

	/**
	 * 打印Debug信息。
	 * Print debug information.
	 * デバッグ情報を印刷します。
	 */
	UPROPERTY(EditDefaultsOnly, Category = Debug)
		bool bDebugPrint = false;


	/**
	 * 忽略在镜头外的。
	 * Ignore those outside the camera view.
	 * カメラの視界外のものを無視します。
	 */
	UPROPERTY()
		bool IgnoreOutofScreen = true;


	/**
	 * 获取世界坐标在屏幕中的坐标。
	 * Get the screen coordinates from the world coordinates.
	 * ワールド座標からスクリーン座標を取得します。
	 * @param World 
	 * @param PlayerController 
	 * @param InLocation 
	 * @param bIsOnScreen 
	 * @return 
	 */
	UFUNCTION()
		FVector2D GetScreenPositionByWorldLocation(UWorld* World, APlayerController* PlayerController, FVector InLocation, bool& bIsOnScreen) const;

	/**
	 * 获取Actor包围盒，根据设定的GetBoxMode。
	 * Get the actor's bounding box based on the specified GetBoxMode.
	 * 設定されたGetBoxModeに基づいてアクターのバウンディングボックスを取得します。
	 * @param Actor 
	 * @return 
	 */
	UFUNCTION()
		FBox GetBox(AActor* Actor) const;

	/**
	 * 获取包围盒时，确认组件是否有效。
	 * Check if the component is valid when getting the bounding box.
	 * バウンディングボックスを取得する際に、コンポーネントが有効かどうかを確認します。
	 * @param Component 
	 * @param Box 
	 */
	UFUNCTION()
		void GetBox_BoxAdd(const UPrimitiveComponent* Component, FBox& Box) const;

	/**
	 * 获取直线上距离某个点最近的点。
	 * Get the closest point on a straight line to a specific point.
	 * 特定の点に最も近い直線上の点を取得します。
	 * @param LineStart 
	 * @param Dir 
	 * @param Point 
	 * @return 
	 */
	UFUNCTION()
		FVector ClosestPointOnInfiniteLine(const FVector& LineStart, const FVector& Dir, const FVector& Point) const;

	/**
	 * 获取Box上距离射线最近的点，粗略获取方法。
	 * Get the closest point on the box to the ray, using an approximate method.
	 * ボックス上のレイに最も近い点を粗い方法で取得します。
	 * @param Box 
	 * @param Start 
	 * @param Dir 
	 * @return 
	 */
	UFUNCTION()
		FVector GetBoxClosestPointWithLine1(const FBox& Box, const FVector& Start, const FVector& Dir) const;

	/**
	 * 获取Box上距离射线最近的点，准确获取方法。
	 * Get the closest point on the box to the ray using an accurate method.
	 * ボックス上のレイに最も近い点を正確な方法で取得します。
	 * @param Box 
	 * @param Start 
	 * @param Dir 
	 * @param IsIntersection 
	 * @return 
	 */
	UFUNCTION()
		FVector GetBoxClosestPointWithLine2(const FBox& Box, const FVector& Start, const FVector& Dir, bool& IsIntersection) const;

	UFUNCTION()
		bool GetDetectionSystemWorldSubsystem(UDetectionSystemWorldSubsystem*& OutSubsystem) const;
};
