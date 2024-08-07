// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleMathBlueprintLibrary.h"

FVector2D UCircleMathBlueprintLibrary::GetPointOnCircle(const FVector2D Centre, const float Radius, const float Theta)
{
	return FVector2D((Radius * FMath::Sin(Theta)) + Centre.X, (Radius * FMath::Cos(Theta)) + Centre.Y);
}

FVector2D UCircleMathBlueprintLibrary::GetCentreOfCircleFrom3Points(const FVector2D A, const FVector2D B, const FVector2D C)
{
	const float a = A.X;
	const float b = A.Y;
	const float c = B.X;
	const float d = B.Y;
	const float f = C.X;
	const float g = C.Y;
	const float q = ((d-b)*(b-g)*(g-d) - (f-a)*(f+a)*(d-b) + (a-c)*(a+c)*(b-g)) / (2 * ((a-c)*(b-g) - (f-a)*(d-b)));
	const float p = ((g+b)*(b-g)*(c-a) + (f-c)*(a-f)*(c-a) - (b+d)*(d-b)*(a-f)) / (2 * ((c-a)*(b-g) - (a-f)*(d-b)));
	return FVector2D(q,p);
}

float UCircleMathBlueprintLibrary::GetRadiusOfCircle(const FVector2D Centre, const FVector2D PointOnCircle)
{
	return FMath::Sqrt(FMath::Square(Centre.X-PointOnCircle.X) + FMath::Square(Centre.Y-PointOnCircle.Y));
}
