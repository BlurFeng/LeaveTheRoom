// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionObjectInterface.h"

// Add default functionality here for any IDetectionObjectInterface functions that are not pure virtual.

bool IDetectionObjectInterface::IsValidTarget_Implementation()
{
	return true;
}

FVector IDetectionObjectInterface::GetDetectionLocation_Implementation()
{
	return FVector();
}

bool IDetectionObjectInterface::GetGameplayTags_Implementation(TArray<FName>& OutGameplayTags)
{
	return false;
}

bool IDetectionObjectInterface::IsCanSelect_Implementation(AActor* Actor)
{
	return true;
}

bool IDetectionObjectInterface::GetGeometryRangeParam_Implementation(EDetectionRangeType& RangeType, FString& Param)
{
	return false;
}
