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
	�Ƚ��������ľ���ֵ�Ĵ�С ������
	@return ����ֵ���������ԭʼֵ
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float AbsMax(const float FloatA, const float FloatB);

	/*
	�ı�һ����ֵ ��������һ����Χ��
	@param value - ���ı�����ĵ�ǰֵ
	@param changValue - �仯ֵ
	@param min - ��С����ֵ
	@param max - �������ֵ
	@param outValue - �ı�����
	@return ��������ֵ�Ƿ����˱仯
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static bool ChangeValueClame(const float Value, const float ChangValue, const float Min, const float Max, float& OutValue);

	/**
	 * ��ȡ������֮��н�
	 * @param dir1 ����1
	 * @param dir2 ����2
	 * @return �н�
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static float VectorAngle(const FVector Dir1, const FVector Dir2);

	/**
	 * ��������limitDir������targetDir�ļн� ������currentDir�޸�֮�������
	 * @param currentDir ��������
	 * @param targetDir Ŀ������
	 * @param limitAngle ���ƽǶ�
	 * @return �ı�������
	*/
	UFUNCTION(BlueprintPure, Category = "UBlurFengLibrary|Maths")
	static FVector DirectionRotateLimit(const FVector CurrentDir, const FVector TargetDir, const float LimitAngle);

	/*����Vector3��ĳ����Χ��*/
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
