// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_QuerierCamera.generated.h"

/**
 * 
 */
UCLASS()
class DETECTIONSYSTEM_API UEnvQueryContext_QuerierCamera : public UEnvQueryContext
{
	GENERATED_UCLASS_BODY()

		virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
