// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CircleMathBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UCircleMathBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Circle Math")
	static FVector2D GetPointOnCircle(FVector2D Centre, float Radius, float Angle);
	
	UFUNCTION(BlueprintPure, Category="Circle Math")
	static FVector2D GetCentreOfCircleFrom3Points(FVector2D A, FVector2D B, FVector2D C);
	
	UFUNCTION(BlueprintPure, Category="Circle Math")
	static float GetRadiusOfCircle(FVector2D Centre, FVector2D PointOnCircle);
	
};
