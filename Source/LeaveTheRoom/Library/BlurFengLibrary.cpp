// Fill out your copyright notice in the Description page of Project Settings.


#include "BlurFengLibrary.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

float UBlurFengLibrary::AbsMax(const float FloatA, const float FloatB) {

	if (FloatA == 0) return FloatB;
	else if (FloatB == 0) return FloatA;
	else if (FMath::Abs(FloatA) > FMath::Abs(FloatB)) {
		return FloatA;
	}
	else {
		return FloatB;
	}
}

bool UBlurFengLibrary::ChangeValueClame(const float Value, const float AddValue, const float Min, const float Max, float& OutValue) {

	if (AddValue > 0) {
		if (Value < Max) {
			OutValue = Value + AddValue;
			if (OutValue > Max)OutValue = Max;

			return true;
		}
	}
	else if (AddValue < 0) {
		if (Value > Min) {
			OutValue = Value + AddValue;
			if (OutValue < Min)OutValue = Min;

			return true;
		}
	}

	OutValue = Value;
	return false;
}

float UBlurFengLibrary::VectorAngle(const FVector Dir1, const FVector Dir2)
{
	float diffVal = FVector::DotProduct(Dir1.GetSafeNormal(), Dir2.GetSafeNormal());
	return FMath::RadiansToDegrees(FMath::Acos(diffVal));
}

FVector UBlurFengLibrary::DirectionRotateLimit(const FVector CurrentDir, const FVector TargetDir, const float LimitAngle)
{
	FVector outDir = CurrentDir;

	float angleDif = VectorAngle(CurrentDir, TargetDir);
	if (angleDif > LimitAngle)
	{
		float limitAngleTemp = LimitAngle > 0 ? LimitAngle : 0;
		float angleTemp = angleDif - limitAngleTemp;

		FVector dirCross = FVector::CrossProduct(CurrentDir, TargetDir);
		outDir = UKismetMathLibrary::RotateAngleAxis(CurrentDir, angleTemp, dirCross);
	}

	return outDir;
}

bool UBlurFengLibrary::Vector3Limit(FVector InV, FVector MaxBounds, FVector MinBounds, FVector& OutV)
{
	OutV = InV;

	if (MaxBounds.X >= MinBounds.X)
	{
		if (OutV.X > MaxBounds.X)OutV.X = MaxBounds.X;
		else if (OutV.X < MinBounds.X)OutV.X = MinBounds.X;
	}

	if (MaxBounds.Y >= MinBounds.Y)
	{
		if (OutV.Y > MaxBounds.Y)OutV.Y = MaxBounds.Y;
		else if (OutV.Y < MinBounds.Y)OutV.Y = MinBounds.Y;
	}

	if (MaxBounds.Z >= MinBounds.Z)
	{
		if (OutV.Z > MaxBounds.Z)OutV.Z = MaxBounds.Z;
		else if (OutV.Z < MinBounds.Z)OutV.Z = MinBounds.Z;
	}

	return !InV.Equals(OutV);
}

void UBlurFengLibrary::SortTArray(TArray<int> GivenList, TArray<int>& UIDList)
{
	GivenList.Sort();
	UIDList = GivenList;
}

bool UBlurFengLibrary::RandomName(const TArray<FName> Names, FName& OutName, int32& OutIndex)
{
	if (Names.IsEmpty()) return false;
	if (Names.Num() == 1) 
	{
		OutName = Names[0];
		return true;
	}

	OutIndex = UKismetMathLibrary::RandomInteger(Names.Num());
	OutName = Names[OutIndex];
	return true;
}

bool UBlurFengLibrary::RandomNameWithout(const TArray<FName> Names, const FName WithoutName, FName& OutName, int32& OutIndex)
{
	if (Names.IsEmpty()) return false;
	if (Names.Num() == 1) 
	{
		OutName = Names[0];
		return !WithoutName.IsEqual(OutName);
	}

	//获取一个随机下标的Name
	RandomName(Names, OutName, OutIndex);

	//如果获取的Name和需要排除的WithoutName相等，那么往后一位取Name，直到不相同或查询了整个数组
	if (OutName.IsEqual(WithoutName))
	{
		int32 counter = 0;
		for (size_t i = OutIndex; i < Names.Num(); i++)
		{
			//遍历了数组但没有找到不相等的Name
			if (counter == Names.Num()) return false;

			//如果Name不相等，返回这个Name
			if (!Names[i].IsEqual(WithoutName))
			{
				OutName = Names[i];
				break;
			}

			//循环到数组最后，再从0开始
			if (i == Names.Num() - 1) i = 0;
			counter++;
		}
	}

	return true;
}

int32 UBlurFengLibrary::RandomIntegerWithout(int32 Max, int32 WithoutInt)
{
	int32 GetInt = UKismetMathLibrary::RandomInteger(Max - 1);

	if (WithoutInt < Max && GetInt >= WithoutInt)
	{
		GetInt++;
	}

	return GetInt;
}
