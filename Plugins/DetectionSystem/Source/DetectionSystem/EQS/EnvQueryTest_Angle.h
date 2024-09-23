// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_Angle.generated.h"

UENUM(BlueprintType)
enum class EEnvTestAngleReferenceDir : uint8
{
	Front,
	Back,
	Left,
	Right,
	Up,
	Down
};

/**
 * 
 */
UCLASS()
class DETECTIONSYSTEM_API UEnvQueryTest_Angle : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
protected:

	/*原点*/
	UPROPERTY(EditDefaultsOnly, Category = Angle)
		TSubclassOf<UEnvQueryContext> Origin;

	/*原点位置偏移*/
	UPROPERTY(EditDefaultsOnly, Category = Angle)
		FVector OriginOffset;

	/*参照方向*/
	UPROPERTY(EditDefaultsOnly, Category = Angle)
		EEnvTestAngleReferenceDir OriginReferenceDir;

	/*限制角度*/
	UPROPERTY(EditDefaultsOnly, Category = Angle)
		float Angle;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
