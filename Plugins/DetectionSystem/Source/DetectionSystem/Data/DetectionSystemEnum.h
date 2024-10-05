// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetectionSystemEnum.generated.h"

/**
 * 探测方向。
 * Detection direction.
 * 検出方向。
 */
UENUM(BlueprintType)
enum class EDetectionDirection : uint8
{
	None,
	Front,
	Back,
	Left,
	Right,
	Up,
	Down,
};

/**
 * 探测范围类型。
 * Detection range type.
 * 検出範囲の種類。
 */
UENUM(BlueprintType)
enum class EDetectionRangeType : uint8
{
	None,
	InvertedCone,
	Ball,
};

///*探测原点类型*/
//UENUM(BlueprintType)
//enum class EDetectionOriginPointType : uint8
//{
//	None,
//
//	/*初始化传参设置的SceneComponent 如果为空则使用自身所有者的Actor位置作为原点*/
//	CenterPointSceneComponent,
//
//	/*自身所有者的Actor*/
//	OwnerActor,
//
//	/*玩家相机*/
//	PlayerCameraManager,
//};