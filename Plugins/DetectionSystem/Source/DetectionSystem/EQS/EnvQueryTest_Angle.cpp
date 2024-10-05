// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_Angle.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Item.h"

UEnvQueryTest_Angle::UEnvQueryTest_Angle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//必须添加此行，否则Test无法工作。
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();

	Origin = UEnvQueryContext_Querier::StaticClass();
	OriginReferenceDir = EEnvTestAngleReferenceDir::Front;
	Angle = 180.f;
}

void UEnvQueryTest_Angle::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	FloatValueMin.BindData(QueryOwner, QueryInstance.QueryID);
	float MinThresholdValue = FloatValueMin.GetValue();

	FloatValueMax.BindData(QueryOwner, QueryInstance.QueryID);
	float MaxThresholdValue = FloatValueMax.GetValue();

	//获取源点位置 这里只获取找到的第一个
	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(Origin, ContextLocations))
	{
		return;
	}
	if (!ContextLocations.IsValidIndex(0)) return;
	FVector OriginPos = ContextLocations[0];

	//获取原点角度
	TArray<FRotator> ContextRotates;
	if (!QueryInstance.PrepareContext(Origin, ContextRotates))
		return;
	if (!ContextRotates.IsValidIndex(0)) return;

	//根据设定偏移 偏移原点位置
	if (OriginOffset != FVector::ZeroVector)
		OriginPos = OriginPos + ContextRotates[0].Quaternion() * OriginOffset;

	//获取原点参照方向
	FVector WorldDir = FVector::ForwardVector;
	switch (OriginReferenceDir)
	{
		case EEnvTestAngleReferenceDir::Front:
			WorldDir = FVector::ForwardVector;
			break;
		case EEnvTestAngleReferenceDir::Back:
			WorldDir = FVector::BackwardVector;
			break;
		case EEnvTestAngleReferenceDir::Left:
			WorldDir = FVector::LeftVector;
			break;
		case EEnvTestAngleReferenceDir::Right:
			WorldDir = FVector::RightVector;
			break;
		case EEnvTestAngleReferenceDir::Up:
			WorldDir = FVector::UpVector;
			break;
		case EEnvTestAngleReferenceDir::Down:
			WorldDir = FVector::DownVector;
			break;
	}
	FVector OriginDir = ContextRotates[0].Quaternion() * WorldDir;

	// loop through all items
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		FVector ItemLocation = QueryInstance.ItemTypeVectorCDO ?
			QueryInstance.ItemTypeVectorCDO->GetItemLocation(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset) :
			FVector::ZeroVector;
		FVector ItemDir = ItemLocation - OriginPos;

		float DiffVal = FVector::DotProduct(OriginDir.GetSafeNormal(), ItemDir.GetSafeNormal());
		if (FMath::IsNaN(DiffVal)) DiffVal = 0.f;
		float AngleItem = FMath::RadiansToDegrees(FMath::Acos(DiffVal));
		bool Allow = Angle >= 360.f || AngleItem <= Angle * 0.5f;
		float Score = ((Angle * 0.5f) - AngleItem) / (Angle * 0.5f);

		//tips:分数为负分的时候 显示看起来不太正常 但是在筛选模式时根据筛选的Min和Max分数限制是能筛掉负分的Item的
		//所以只在TestPurpose有Filter时 设置负分
		if (TestPurpose == EEnvTestPurpose::Type::Score && Score < 0.f) Score = 0.f;

		It.SetScore(TestPurpose, FilterType, Score, MinThresholdValue, MaxThresholdValue);
	}
}

FText UEnvQueryTest_Angle::GetDescriptionTitle() const
{
	FString DirDesc;
	switch (OriginReferenceDir)
	{
		case EEnvTestAngleReferenceDir::Front:
			DirDesc = TEXT(" OriginReferenceDir Front");
			break;
		case EEnvTestAngleReferenceDir::Back:
			DirDesc = TEXT(" OriginReferenceDir Back");
			break;
		case EEnvTestAngleReferenceDir::Left:
			DirDesc = TEXT(" OriginReferenceDir Left");
			break;
		case EEnvTestAngleReferenceDir::Right:
			DirDesc = TEXT(" OriginReferenceDir Right");
			break;
		case EEnvTestAngleReferenceDir::Up:
			DirDesc = TEXT(" OriginReferenceDir Up");
			break;
		case EEnvTestAngleReferenceDir::Down:
			DirDesc = TEXT(" OriginReferenceDir Down");
			break;
		default:
			break;
	}

	return FText::FromString(FString::Printf(TEXT("%s: %s and %s"),
		*Super::GetDescriptionTitle().ToString(), *DirDesc, *FString::SanitizeFloat(Angle)));
}

FText UEnvQueryTest_Angle::GetDescriptionDetails() const
{
	return FText();
}
