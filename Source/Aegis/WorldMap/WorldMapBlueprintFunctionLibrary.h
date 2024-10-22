// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WorldMapBlueprintFunctionLibrary.generated.h"

UENUM(BlueprintType)
enum EMapNodeType
{
	MinorEnemy,
	EliteEnemy,
	RestSite,
	Shop,
	BossEnemy,
};

USTRUCT(BlueprintType)
struct FMapNodeCoordinate
{
	GENERATED_BODY()

	FMapNodeCoordinate() {}

	FMapNodeCoordinate(const int32 Layer, const int32 Row)
	{
		this->Layer = Layer;
		this->Row = Row;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Layer = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Row = 0;
	
	bool operator==(const FMapNodeCoordinate& RHSMapNodeCoordinate) const
	{
		return (this->Layer == RHSMapNodeCoordinate.Layer && this->Row == RHSMapNodeCoordinate.Row);
	}

	bool operator!=(const FMapNodeCoordinate& RHSMapNodeCoordinate) const
	{
		return (this->Layer != RHSMapNodeCoordinate.Layer || this->Row != RHSMapNodeCoordinate.Row);
	}
};

FORCEINLINE uint32 GetTypeHash(const FMapNodeCoordinate& MapNodeCoord)
{
	return FCrc::MemCrc32(&MapNodeCoord, sizeof(FMapNodeCoordinate));
}


USTRUCT(BlueprintType)
struct FMapNode
{
	GENERATED_BODY()

	FMapNode() {}

	FMapNode(const EMapNodeType InNodeType, const FMapNodeCoordinate InCoordinate)
	{
		this->NodeType = InNodeType;
		this->NodeCoordinate = InCoordinate;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EMapNodeType> NodeType = MinorEnemy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<FMapNodeCoordinate> OutGoingConnections = TSet<FMapNodeCoordinate>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapNodeCoordinate NodeCoordinate = FMapNodeCoordinate();

	bool operator==(const FMapNode& RHSMapNode) const
	{
		return (this->NodeCoordinate == RHSMapNode.NodeCoordinate);
	}
};

FORCEINLINE uint32 GetTypeHash(const FMapNode& MapNode)
{
	return FCrc::MemCrc32(&MapNode, sizeof(FMapNode));
}


USTRUCT(BlueprintType)
struct FWorldMapCreationConfig
{
	GENERATED_BODY()

	FWorldMapCreationConfig()
	{
		StartingLayerNodesCount = 0;
		PreBossLayerNodesCount = 0;
		NumOfExtraPaths = 0;
		LayersCount = 0;
		GridRows = 0;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartingLayerNodesCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PreBossLayerNodesCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumOfExtraPaths;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LayersCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GridRows;

	int32 GetGridHeight() const
	{
		return FMath::Max(StartingLayerNodesCount, PreBossLayerNodesCount);
	}
};

USTRUCT(BlueprintType)
struct FWorldMapData
{
	GENERATED_BODY()
	FWorldMapData()
	{
		MapNodes = TArray<FMapNode>();
		HeadNode = FMapNode();
		ExploredNodes = TArray<FMapNode>();
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMapNode> MapNodes;
	int32 GetMapNodeIndex(const FMapNodeCoordinate Coord)
	{
		for (int i = 0; i < MapNodes.Num(); i++)
		{
			if (MapNodes[i].NodeCoordinate == Coord) { return i; }
		}
		return -1;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapNode HeadNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMapNode> ExploredNodes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FWorldMapCreationConfig InitialConfig;
};


/**
 * 
 */
UCLASS()
class AEGIS_API UWorldMapBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FWorldMapData GenerateWorldMapData(const FWorldMapCreationConfig MapConfig);

	UFUNCTION(BlueprintCallable)
	static FVector2D GetMapPosition(const FMapNodeCoordinate NodeCoordinate, const float ContainerHeight, const int32 RowCount);

	UFUNCTION(BlueprintCallable)
	static FWorldMapData TravelToNode(const FMapNode Node, FWorldMapData WorldMapData);

	UFUNCTION(BlueprintCallable)
	static FWorldMapData ExploreHeadNode(FWorldMapData WorldMapData);
	
	UFUNCTION(BlueprintCallable)
	static float GetAngleBetweenTwoPoints(const FVector2D Origin, const FVector2D Destination);

private:
	static FWorldMapData CreatePrototypeWorldMapData();

	UFUNCTION(Blueprintable)
	static void GeneratePath(FWorldMapData& WorldMapData, const FMapNodeCoordinate StartNodeCoordinate, const FMapNodeCoordinate EndNodeCoordinate);
	
	UFUNCTION(Blueprintable)
	static void ResolveCrossPaths(const FWorldMapData& WorldMapData);
};
