// Fill out your copyright notice in the Description page of Project Settings.


#include "DetectionSystemDebugHUD.h"

#include "DrawDebugHelpers.h"

ADetectionSystemDebugHUD::ADetectionSystemDebugHUD()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADetectionSystemDebugHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADetectionSystemDebugHUD::OnEndDraw()
{
	RemoveAllDebugStrings();

	if (bDrawDrawEQTScreenDistanceRange)
	{
		DrawDebugCanvas2DCircle(Canvas, DrawEQTScreenDistanceRange_Center, DrawEQTScreenDistanceRange_Radius, 24, FLinearColor::Blue, 1.f);

		for (int i = 0; i < DrawEQTScreenRange_ItemPoints.Num(); i++)
		{
			FDetectionSystemDebug2DBoxInfo detectionSystemDebug2DBoxInfo = DrawEQTScreenRange_ItemPoints[i];
			FVector2D point = detectionSystemDebug2DBoxInfo.Position;
			FLinearColor color = detectionSystemDebug2DBoxInfo.Color;
			float size = detectionSystemDebug2DBoxInfo.Size;
			FBox2D pointBox = FBox2D(FVector2D(point.X - size, point.Y - size), FVector2D(point.X + size, point.Y + size));
			DrawDebugCanvas2DBox(Canvas, pointBox, color);
		}

		UWorld* world = GetWorld();
		if (world)
		{
			for (int i = 0; i < DrawEQTScreenRange_Score.Num(); i++)
			{
				FDetectionSystemDebugTextInfo detectionSystemDebugTextInfo = DrawEQTScreenRange_Score[i];
				AActor* BaseAct = world->GetWorldSettings();
				AddDebugText(
					detectionSystemDebugTextInfo.String, BaseAct, 1.f, 
					detectionSystemDebugTextInfo.Location, 
					detectionSystemDebugTextInfo.Location, 
					detectionSystemDebugTextInfo.Color, true, true, false, nullptr, 20.f, false);
			}
		}
	}
}

void ADetectionSystemDebugHUD::DrawDebugHUD(UCanvas* InCanvas, APlayerController*)
{
	Canvas = InCanvas;
	if (!Canvas)
	{
		return;
	}

	UPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!LocalPlayer)
	{
		return;
	}

	APlayerController* PC = LocalPlayer->PlayerController;
	if (!PC)
	{
		return;
	}

	GetWorld()->GetGameViewport()->OnEndDraw().AddUObject(this, &ADetectionSystemDebugHUD::OnEndDraw);
}

UCanvas* ADetectionSystemDebugHUD::GetCanvas() 
{
	return Canvas;
}

void ADetectionSystemDebugHUD::SetDrawEQTScreenRange(const FVector2D& Center, float Radius)
{
	bDrawDrawEQTScreenDistanceRange = true;

	DrawEQTScreenDistanceRange_Center = Center;
	DrawEQTScreenDistanceRange_Radius = Radius;
}

void ADetectionSystemDebugHUD::DisableDrawEQTScreenRange()
{
	if (!bDrawDrawEQTScreenDistanceRange) return;

	bDrawDrawEQTScreenDistanceRange = false;
}

void ADetectionSystemDebugHUD::SetDrawEQTScreenRange_ItemPoint(const FVector2D& Point, FLinearColor Color)
{
	DrawEQTScreenRange_ItemPoints.Add(FDetectionSystemDebug2DBoxInfo(Point, Color, 4.f));
}

void ADetectionSystemDebugHUD::SetDrawEQTScreenRange_Clear()
{
	DrawEQTScreenRange_ItemPoints.Empty();
	DrawEQTScreenRange_Score.Empty();
}

void ADetectionSystemDebugHUD::SetDrawEQTScreenRange_Score(const FString& String, const FVector& Location, FColor Color)
{
	DrawEQTScreenRange_Score.Add(FDetectionSystemDebugTextInfo(String, Location, Color));
}