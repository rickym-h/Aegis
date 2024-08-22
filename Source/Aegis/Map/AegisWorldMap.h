// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AegisMap.h"
#include "AegisWorldMap.generated.h"

/**
 * 
 */
UCLASS()
class AEGIS_API UAegisWorldMap : public UAegisMap
{
	GENERATED_BODY()

public:
	TMap<FTileCoord, TSet<FTileCoord>> NodeGraph;
	TArray<FTileCoord> ExploredNodes;
	

protected:
};
