// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlurFengLibrary.generated.h"

/**
 * 常用方法库。
 * Commonly used method library.
 * よく使われるメソッドライブラリ。
 */
UCLASS()
class LEAVETHEROOM_API UBlurFengLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	//-------- Maths --------

	/*
	* 比较两个数的绝对值的大小，并返回更大的那个。
	* Compare the absolute values of two numbers and return the larger one.
	* 2つの数の絶対値を比較し、より大きい方を返します。
	* @return 
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float AbsMax(const float FloatA, const float FloatB);

	/**
	 * 改变一个数值，并限制在一定范围内。
	 * Change a value and constrain it within a certain range.
	 * 値を変更し、特定の範囲内に制約します。
	 * @param value - 被改变的数的当前值
	 * @param changValue - 变化值
	 * @param min - 最小限制值
	 * @param max - 最大限制值
	 * @param outValue - 改变后的数
	 * @return 操作的数值是否发生了变化
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static bool ChangeValueClame(const float Value, const float ChangValue, const float Min, const float Max, float& OutValue);

	/**
	 * 获取两向量之间夹角。
	 * Get the angle between two vectors.
	 * 2つのベクトルの間の角度を取得します。
	 * @param dir1 向量1
	 * @param dir2 向量2
	 * @return 夹角
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float VectorAngle(const FVector Dir1, const FVector Dir2);

	/**
	 * 限制向量CurrentDir和向量TargetDir的夹角，并返回CurrentDir被修改之后的向量。
	 * Limit the angle between the vector CurrentDir and the vector TargetDir, and return the modified CurrentDir vector.
	 * ベクトルCurrentDirとベクトルTargetDirの間の角度を制限し、修正されたCurrentDirベクトルを返します。
	 * @param currentDir 限制向量
	 * @param targetDir 目标向量
	 * @param limitAngle 限制角度
	 * @return 改变后的向量
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static FVector DirectionRotateLimit(const FVector CurrentDir, const FVector TargetDir, const float LimitAngle);

	/**
	 * 限制Vector在某个范围内。
	 * Constrain the vector within a certain range.
	 * ベクトルを特定の範囲内に制約します。
	 * @param InV 
	 * @param MaxBounds 
	 * @param MinBounds 
	 * @param OutV 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static bool Vector3Limit(FVector InV, FVector MaxBounds, FVector MinBounds, FVector& OutV);

	UFUNCTION(BlueprintCallable, Category = "UBlurFengLibrary|Maths")
	static void SortTArray(const TArray<int> GivenList, TArray<int>& UIDList);




	//-------- UE Data --------
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random")
	static bool RandomName(const TArray<FName> Names, FName& OutName, int32& OutIndex);

	/**
	 * 在传入的Names数组中，除去传入的WithoutName后返回一个随机Name值。
	 * Return a random Name value from the given Names array, excluding the provided WithoutName.
	 * 渡されたNames配列から、指定されたWithoutNameを除外してランダムなName値を返します。
	 * @param Names 
	 * @param WithoutName 
	 * @param OutName 
	 * @param OutIndex 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random")
	static bool RandomNameWithout(const TArray<FName> Names, const FName WithoutName, FName& OutName, int32& OutIndex);

	/**
	 * 在限制的范围内，除去传入的WithoutInt后返回一个随机int值。
	 * Return a random int value within the constrained range, excluding the given WithoutInt.
	 * 制約された範囲内で、指定されたWithoutIntを除外してランダムな整数値を返します。
	 * @param Max 
	 * @param WithoutInt 
	 * @return 
	 */
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Random", meta = (NotBlueprintThreadSafe))
	static int32 RandomIntegerWithout(int32 Max, int32 WithoutInt);
};
