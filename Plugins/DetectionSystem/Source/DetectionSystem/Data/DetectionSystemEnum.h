// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetectionSystemEnum.generated.h"

/*探测方向*/
UENUM(BlueprintType)
enum class EDetectionDirection : uint8
{
	None,

	/*前方*/
	Front,
	/*后方*/
	Back,
	/*左方*/
	Left,
	/*右方*/
	Right,
	/*上方*/
	Up,
	/*下方*/
	Down,
};

/*探测范围类型*/
UENUM(BlueprintType)
enum class EDetectionRangeType : uint8
{
	None,

	/*
	倒置球底圆锥
	探测范围 实现
	GetGeometryRangeParam接口 未实现
	*/
	InvertedCone,

	/*
	球形
	探测范围 实现
	GetGeometryRangeParam接口 参数格式 "半径"
	*/
	Ball,
};

/*探测原点类型*/
UENUM(BlueprintType)
enum class EDetectionOriginPointType : uint8
{
	None,

	/*初始化传参设置的SceneComponent 如果为空则使用自身所有者的Actor位置作为原点*/
	CenterPointSceneComponent,

	/*自身所有者的Actor*/
	OwnerActor,

	/*玩家相机*/
	PlayerCameraManager,
};