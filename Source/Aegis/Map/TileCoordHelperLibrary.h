// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TileCoordHelperLibrary.generated.h"


USTRUCT(BlueprintType, meta = (HasNativeMake = "Aegis.TileCoordHelperLibrary.MakeTileCoordStruct"))
struct FTileCoord
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Q;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int R;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int S;

	FTileCoord()
	{
		Q = 0;
		R = 0;
		S = 0;
	}

	FTileCoord(const int InQ, const int InR)
	{
		Q = InQ;
		R = InR;
		S = -Q-R;
	}

	FTileCoord Copy() const
	{
		return FTileCoord(this->Q, this->R);
	}

	FString ToString() const
	{
		return FString("Q=").Append(FString::FromInt(Q)).Append(" R=").Append(FString::FromInt(R)).Append(" S=").Append(FString::FromInt(S));
	}

	bool IsValid() const
	{
		return S == -Q-R;
	}

	FVector ToWorldLocation(const float TileRadius = 100) const
	{
		const float Right = TileRadius * ((FMath::Sqrt(3.f) * this->Q) + ((FMath::Sqrt(3.f)/2) * this->R));
		const float Forward = TileRadius * (1.5 * this->R);
		return FVector(Forward, Right, 0);
		// const float VerticalSpacing = 1.5 * TileRadius;
		// const float HorizontalSpacing = FMath::Sqrt(3.f) * TileRadius;
		//
		// const FVector OffsetQ = FVector(0, HorizontalSpacing, 0);
		// const FVector OffsetR = FVector(VerticalSpacing, HorizontalSpacing/2, 0);
		//
		// return (R * OffsetR) + (Q * OffsetQ);
		
	}

	// Calculate the HexDistance between two tiles. Similar to Manhattan distance but uses three axis (QRS) instead of two (XY) 
	static int HexDistance(const FTileCoord TileA, const FTileCoord TileB)
	{
		const FTileCoord DistanceVector = TileA - TileB;
		return FMath::Max3(FMath::Abs(DistanceVector.Q), FMath::Abs(DistanceVector.R), FMath::Abs(DistanceVector.S));
	}

	static TArray<FTileCoord> GetTilesInRadius(FTileCoord Origin, int Radius)
	{
		TArray<FTileCoord> TilesInRadius;
		for (int Q = -Radius; Q <= Radius; Q++)
		{
			for (int R = FMath::Max(-Radius, -Q-Radius); R <= FMath::Min(Radius, -Q+Radius); R++)
			{
				TilesInRadius.Add(FTileCoord(Q, R) + Origin);
			}
		}
		return TilesInRadius;
	}

	// static FTileCoord FromLocation(const FVector& Location)
	// {
	// 	const float Right = Location.Y;
	// 	const float Forward = Location.X;
	// 	
	// 	const float Qf = ((FMath::Sqrt(3.f)/3.f * Right) - ((1.f/3.f) * Forward)) / 100.f;
	// 	const float Rf = ((2.f/3.f) * Right) / 100.f;
	// 	const float Sf = -Qf-Rf;
	
	// 	// integrate axial_round function
	// 	int Q = FMath::RoundToInt(Qf);
	// 	int R = FMath::RoundToInt(Rf);
	// 	int S = FMath::RoundToInt(Sf);
	//
	// 	const float QDiff = FMath::Abs(static_cast<float>(Q)-Qf);
	// 	const float RDiff = FMath::Abs(static_cast<float>(R)-Rf);
	// 	const float SDiff = FMath::Abs(static_cast<float>(S)-Sf);
	//
	// 	if ((QDiff > RDiff) && (QDiff > SDiff))
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Q Updating!"))
	// 		Q = -R-S;
	// 	} else if (RDiff > SDiff)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("R Updating!"))
	// 		R = -Q-S;
	// 	} else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("S Updating!"))
	// 		S = -Q-R;
	// 	}
	// 	return FTileCoord(Q,R);
	// }

	bool operator==(const FTileCoord &RHSTile) const
	{
		return (this->IsValid() && RHSTile.IsValid() && (this->Q == RHSTile.Q) && (this->R == RHSTile.R));
	}
	FTileCoord operator+(const FTileCoord &RHSTile) const
	{
		return FTileCoord(this->Q + RHSTile.Q, this->R + RHSTile.R);
	}
	FTileCoord operator-(const FTileCoord &RHSTile) const
	{
		return FTileCoord(this->Q - RHSTile.Q, this->R - RHSTile.R);
	}
};
FORCEINLINE uint32 GetTypeHash(const FTileCoord& Coord)
{
	return FCrc::MemCrc32(&Coord, sizeof(FTileCoord));
}


UCLASS()
class AEGIS_API UTileCoordHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "TileCoord", meta = (BlueprintThreadSafe))
	static FTileCoord MakeTileCoordStruct(int Q, int R);
};
