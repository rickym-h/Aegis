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
};


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
	TArray<FMapNodeCoordinate> OutGoingConnections = TArray<FMapNodeCoordinate>();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapNodeCoordinate NodeCoordinate = FMapNodeCoordinate();

	bool operator==(const FMapNode& RHSMapNode) const
	{
		return (this->NodeCoordinate == RHSMapNode.NodeCoordinate);
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapNode HeadNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMapNode> ExploredNodes;
};


USTRUCT(BlueprintType)
struct FWorldMapCreationConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartingLayerNodesCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 PreBossLayerNodesCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 LayersCount;

	int32 GetGridHeight() const
	{
		return FMath::Max(StartingLayerNodesCount, PreBossLayerNodesCount);
	}
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
	static FWorldMapData ExploreNode(const FMapNode Node, FWorldMapData WorldMapData);
	

	UFUNCTION(BlueprintCallable)
	static float GetAngleBetweenTwoPoints(const FVector2D Origin, const FVector2D Destination);
};
