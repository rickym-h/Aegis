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
		S = -Q - R;
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
		return S == -Q - R;
	}

	FVector ToWorldLocation(const float TileRadius = 100) const
	{
		const float Right = TileRadius * ((FMath::Sqrt(3.f) * this->Q) + ((FMath::Sqrt(3.f) / 2.f) * this->R));
		const float Forward = TileRadius * (1.5f * this->R);
		return FVector(Forward, Right, 0);
	}

	static FTileCoord AxialRound(const float InQ, const float InR)
	{
		const float InS = -InQ - InR;
		int Q = FMath::RoundHalfToZero(InQ);
		int R = FMath::RoundHalfToZero(InR);
		int S = FMath::RoundHalfToZero(InS);

		float q_diff = abs(Q - InQ);
		float r_diff = abs(R - InR);
		float s_diff = abs(S - InS);

		if (q_diff > r_diff && q_diff > s_diff)
		{
			Q = -R - S;
		}
		else if (r_diff > s_diff)
		{
			R = -Q - S;
		}
		else
		{
			S = -Q - R;
		}

		return FTileCoord(Q, R);
	}

	static FTileCoord PixelToHex(const FVector& PixelPoint, const float TileRadius = 100)
	{
		const float q = (sqrt(3) / 3 * PixelPoint.Y - 1. / 3 * PixelPoint.X) / TileRadius;
		const float r = (2. / 3 * PixelPoint.X) / TileRadius;
		return AxialRound(q, r);
	}

	// Calculate the HexDistance between two tiles. Similar to Manhattan distance but uses three axis (QRS) instead of two (XY) 
	static int HexDistance(const FTileCoord TileA, const FTileCoord TileB)
	{
		const FTileCoord DistanceVector = TileA - TileB;
		return FMath::Max3(FMath::Abs(DistanceVector.Q), FMath::Abs(DistanceVector.R), FMath::Abs(DistanceVector.S));
	}

	static int HexDistanceToTiles(const TArray<FTileCoord>& Array, const FTileCoord Key){
		int MinHexDistance = INT_MAX;
		for (const FTileCoord Element : Array)
		{
			int Distance = HexDistance(Element, Key);
			MinHexDistance = FMath::Min(Distance, MinHexDistance);
		}
		return MinHexDistance;
	}

	static TArray<FTileCoord> GetTilesInRadius(const FTileCoord Origin, const int Radius)
	{
		TArray<FTileCoord> TilesInRadius;
		for (int Q = -Radius; Q <= Radius; Q++)
		{
			for (int R = FMath::Max(-Radius, -Q - Radius); R <= FMath::Min(Radius, -Q + Radius); R++)
			{
				TilesInRadius.Add(FTileCoord(Q, R) + Origin);
			}
		}
		return TilesInRadius;
	}

	static TArray<FTileCoord> GetTilesInRadius(const TArray<FTileCoord>& Origins, const int Radius)
	{
		TSet<FTileCoord> Tiles;

		for (const FTileCoord OriginTile : Origins)
		{
			Tiles.Append(GetTilesInRadius(OriginTile, Radius));
		}

		return Tiles.Array();
	}


	bool operator==(const FTileCoord& RHSTile) const
	{
		return (this->IsValid() && RHSTile.IsValid() && (this->Q == RHSTile.Q) && (this->R == RHSTile.R));
	}

	bool operator!=(const FTileCoord& RHSTile) const
	{
		return (this->IsValid() && RHSTile.IsValid() && ((this->Q != RHSTile.Q) || (this->R != RHSTile.R)));
	}

	FTileCoord operator+(const FTileCoord& RHSTile) const
	{
		return FTileCoord(this->Q + RHSTile.Q, this->R + RHSTile.R);
	}

	FTileCoord operator-(const FTileCoord& RHSTile) const
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
