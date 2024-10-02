// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameActor.generated.h"

/**
 * 这是用于游戏的基本Actor类型，游戏项目资源中所有的子类Actor都应该继承此类。除非你明确你不需要继承此类。
 * 此类用于实现为项目所有Actor服务的基本功能。不应当设计具体的业务逻辑。
 */
UCLASS()
class LEAVETHEROOM_API AGameActor : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameActor();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameActor")
	FGameplayTagContainer GameplayTagContainer;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 通过 IGameplayTagAssetInterface 继承
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
};
