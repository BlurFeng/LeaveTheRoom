// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_QuerierCamera.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "AISystem.h"
#include "VisualLogger/VisualLogger.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

UEnvQueryContext_QuerierCamera::UEnvQueryContext_QuerierCamera(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UEnvQueryContext_QuerierCamera::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	
	APlayerController* controller;

	//通过pawn获取PlayerController。
	//Get the PlayerController through the pawn.
	//ポーンを通じてPlayerControllerを取得します。
	APawn* pawn = Cast<APawn>(QueryOwner);
	if (pawn != nullptr)
	{
		controller = Cast<APlayerController>(pawn->GetController());
	}
	else
		controller = Cast<APlayerController>(QueryOwner);

	//通过PlayerController获取Camera。
	//Get the Camera through the PlayerController.
	//PlayerControllerを通じてカメラを取得します。
	if (controller != nullptr)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, controller->PlayerCameraManager);
		return;
	}
	
	UE_CVLOG(Cast<APlayerCameraManager>(QueryOwner) == nullptr, QueryOwner, LogEQS, Warning, TEXT("UEnvQueryContext_QuerierCamera can't get PlayerCameraManager!"));
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, QueryOwner);
}