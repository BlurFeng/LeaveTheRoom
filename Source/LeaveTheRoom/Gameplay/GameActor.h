// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameActor.generated.h"

/**
 * 用于游戏的基本Actor类型。在原生Actor和业务之间做一次隔离。次Actor实现一些非业务的通用功能。
 * Basic Actor type used for the game. This Actor acts as an isolation layer between the native Actor and business logic. It implements some non-business generic functionalities.
 * ゲームに使用される基本的なActorタイプです。このActorは、ネイティブActorとビジネスロジックの間の隔離層として機能します。一部の非ビジネスの汎用機能を実装します。
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
