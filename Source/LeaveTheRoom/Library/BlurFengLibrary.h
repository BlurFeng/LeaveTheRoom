// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlurFengLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LEAVETHEROOM_API UBlurFengLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	//-------- Maths --------

	/*
	比较两个数的绝对值的大小 并返回
	@return 绝对值更大的数的原始值
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float AbsMax(const float FloatA, const float FloatB);

	/*
	改变一个数值 并限制在一定范围内
	@param value - 被改变的数的当前值
	@param changValue - 变化值
	@param min - 最小限制值
	@param max - 最大限制值
	@param outValue - 改变后的数
	@return 操作的数值是否发生了变化
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static bool ChangeValueClame(const float Value, const float ChangValue, const float Min, const float Max, float& OutValue);

	/**
	 * 获取两向量之间夹角
	 * @param dir1 向量1
	 * @param dir2 向量2
	 * @return 夹角
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float VectorAngle(const FVector Dir1, const FVector Dir2);

	/**
	 * 限制向量limitDir和向量targetDir的夹角 并返回currentDir修改之后的向量
	 * @param currentDir 限制向量
	 * @param targetDir 目标向量
	 * @param limitAngle 限制角度
	 * @return 改变后的向量
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static FVector DirectionRotateLimit(const FVector CurrentDir, const FVector TargetDir, const float LimitAngle);

	/*限制Vector3在某个范围内*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static bool Vector3Limit(FVector InV, FVector MaxBounds, FVector MinBounds, FVector& OutV);

	UFUNCTION(BlueprintCallable, Category = "UBlurFengLibrary|Maths")
	static void SortTArray(const TArray<int> GivenList, TArray<int>& UIDList);




	//-------- UE Data --------
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random")
	static bool RandomName(const TArray<FName> Names, FName& OutName, int32& OutIndex);

	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random")
	static bool RandomNameWithout(const TArray<FName> Names, const FName WithoutName, FName& OutName, int32& OutIndex);

	/**
	 * @brief Returns a uniformly distributed random number between 0 and Max - 1.Without input Integer.
	 * @param Max 
	 * @param WithoutInt 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random", meta = (NotBlueprintThreadSafe))
	static int32 RandomIntegerWithout(int32 Max, int32 WithoutInt);
};
