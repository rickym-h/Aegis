// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "MapTile.h"
#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Structures/NexusBuilding.h"

void UAegisMapFactory::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!GetWorld()) { return; }
	
	TileHeight = 2*TileRadius;
	TileWidth = FMath::Sqrt(3.f) * TileRadius;

	VerticalSpacing = 1.5 * TileRadius;
	HorizontalSpacing = TileWidth;

	OffsetQ = FVector(0, HorizontalSpacing, 0);
	OffsetR = FVector(VerticalSpacing, HorizontalSpacing/2, 0);
}

UAegisMap* UAegisMapFactory::GenerateMap(const int PathClusterLength, const int PathsCount) const
{
	// Check the input parameters are valid
	if (PathClusterLength < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GenerateMap() - PathClusterLength too short."))
	}
	// Check factory classes are valid
	if (!GrassTileBP)
	{
		UE_LOG(LogTemp, Error, TEXT("GrassTileBP not valid"))
		return NewObject<UAegisMap>();
	}

	// Generate Map Data
	const TMap<FTileCoord, FTileCoord> PathClusters = GeneratePath(PathClusterLength, PathsCount, 0);
	const TMap<FTileCoord, FTileCoord> PathRoute = UPathGenerationBlueprintLibrary::GetFullPathFromClusters(PathClusters);
	
	const TMap<FTileCoord, AMapTile*> MapTiles = GenerateMapTiles(PathRoute, PathClusters);

	for (TTuple<FTileCoord, FTileCoord> Entry : PathRoute)
	{
		if (AMapTile* Tile = MapTiles[Entry.Key])
		{
			Tile->ToggleShowGradients();
		}
	}

	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(PathRoute);
	
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	// Create Map
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);
	Map->PopulateMapData(MapTiles, PathRoute, PathStartTileCoords, NexusBuilding);

	// Post Map creation settings
	for (AMapTile* Tile : Map->GetTiles())
	{
		Tile->TilesToEnd = Map->GetNumOfTilesToEnd(Tile->TileCoord);
	}

	return Map;
}



TMap<FTileCoord, AMapTile*> UAegisMapFactory::GenerateMapTiles(const TMap<FTileCoord, FTileCoord>& PathRoute, const TMap<FTileCoord, FTileCoord>& PathClusters) const
{
	// Random offset to use for perlin noise generation (to be seemingly unique every time) as FMath::PerlinNoise2D is not seeded
	const FVector2D RandomNoiseOffset = FVector2D(FMath::FRandRange(-100000.f, 1000000.f));

	TSet<FTileCoord> CoordsInRangeOfPath;
	// Ensure all path tiles are generated
	for (TTuple<FTileCoord, FTileCoord> Entry : PathRoute)
	{
		TArray<FTileCoord> TilesInRadius = FTileCoord::GetTilesInRadius(Entry.Key, 1);
		for (FTileCoord TileCoord : TilesInRadius)
		{
			CoordsInRangeOfPath.Add(TileCoord);
		}
	}
	// Generate tiles near each path cluster
	for (TTuple<FTileCoord, FTileCoord> Entry : PathClusters)
	{
		for (FTileCoord Cluster : UPathGenerationBlueprintLibrary::GetClustersInRange(Entry.Key, 1))
		{
			TArray<FTileCoord> TilesInRadius = FTileCoord::GetTilesInRadius(Cluster, 2);
			for (FTileCoord TileCoord : TilesInRadius)
			{
				CoordsInRangeOfPath.Add(TileCoord);
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GenerateMapTiles() - Generating %d tiles."), CoordsInRangeOfPath.Num())
	
	TMap<FTileCoord, AMapTile*> MapTiles;
	for (const FTileCoord ThisTileCoord : CoordsInRangeOfPath.Array())
	{
		int Q = ThisTileCoord.Q;
		int R = ThisTileCoord.R;
		
		// World location of tile BP
		FVector Location = ThisTileCoord.ToWorldLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
			
		FActorSpawnParameters SpawnInfo;

		// TODO decide if it should be a GrassTileBP or a PathTile
		// (for now it is just visualised by toggling debug gradients at end of tile creation)

		// Spawn actual tile BP in world
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(GrassTileBP, Location, Rotation);

		FVector2D NoiseSampleLoc = FVector2D(Location.X/100, Location.Y/100) + RandomNoiseOffset;
		float NoiseValue = FMath::PerlinNoise2D(NoiseSampleLoc);
		NoiseValue = (NoiseValue+1)/2; // Map from -1-1 to 0-1

		// Curve Temp using S curve
		NoiseValue = 1/(1+FMath::Pow(NoiseValue/(1-NoiseValue), -3));
		// Square to bring lower noise values lower (So pathing might opt for longer routes with less noise rather than going through higher noise tiles)
		NoiseValue = FMath::Pow(NoiseValue, 2);

		// Clamp the noise to between 0 and 1 in case scaling has placed noise out of bounds
		NoiseValue = FMath::Min(NoiseValue, 1.f);
		NoiseValue = FMath::Max(NoiseValue, 0.f);
			
		Tile->TileCoord = ThisTileCoord.Copy();
		Tile->PathingGradient = NoiseValue;
			
		//Map->AddTileToMap(ThisTileCoord, Tile);
		MapTiles.Add(ThisTileCoord, Tile);
	}
	
	return MapTiles;
}

TMap<FTileCoord, FTileCoord> UAegisMapFactory::GeneratePath(const int PathLengthInClusters, const int PathsCount, const int BranchesCount)
{
	if (PathsCount < 1 || PathsCount > 6)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - PathsCount out of range."))
	}
	
	// Init a stack of Paths (stack for DFS, queue for BFS)
	TArray<TMap<FTileCoord,FTileCoord>> PossiblePaths;

	// Add the first paths onto the stack
	TArray<FTileCoord> PossibleFirstClusters = UPathGenerationBlueprintLibrary::GetAdjacentClustersRandom(FTileCoord());
	TMap<FTileCoord,FTileCoord> AFirstPath;
	AFirstPath.Add(FTileCoord(), FTileCoord());
	for (int i = 0; i < PathsCount; i++)
	{
		AFirstPath.Add(PossibleFirstClusters[i], FTileCoord());
	}
	PossiblePaths.Push(AFirstPath);
	
	// Recurse
	int Count = 0;
	while (PossiblePaths.Num() > 0)
	{
		Count++;
		TMap<FTileCoord,FTileCoord> Path = PossiblePaths.Pop();
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - %d - Testing path of length: %d"), Count, Path.Num())

		// If the path is in the goal state, return it.
		if (UPathGenerationBlueprintLibrary::PathMapIsValid(Path, PathLengthInClusters, PathsCount, BranchesCount))
		{
			return Path;
		}

		// Enumerate all possible next states, and add them to the stack
		if (Path.Num()-1 > PathLengthInClusters) { continue; }

		// Get the node to generate the next path from (shortest distance to centre)
		FTileCoord NodeToBranchFrom = UPathGenerationBlueprintLibrary::GetClosestEndToCentre(Path);

		// Generate all possible nodes from this path
		TArray<FTileCoord> PossibleAdjacentClusters = UPathGenerationBlueprintLibrary::GetAdjacentClustersRandom(NodeToBranchFrom);
		TArray<FTileCoord> AdjacentClusters;
		for (FTileCoord PossibleNextNode : PossibleAdjacentClusters)
		{
			if (!Path.Contains(PossibleNextNode)) { AdjacentClusters.Add(PossibleNextNode); }
		}
		
		for (FTileCoord PossibleNextNode : AdjacentClusters)
		{
			// Generate a new path map, and add to the queue
			TMap<FTileCoord, FTileCoord> NewPathCopy = UPathGenerationBlueprintLibrary::DeepCopyPath(Path);
			NewPathCopy.Add(PossibleNextNode, NodeToBranchFrom);

			PossiblePaths.Push(NewPathCopy);
		}
	}

	UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - Could not find path!"))
	TMap<FTileCoord,FTileCoord> EmptyPath;
	return EmptyPath;
	
	// const int MapRadiusInTiles = PathLengthInClusters * 3;
	//
	// TMap<FTileCoord, FTileCoord> PathRoute;
	// for (int Q = MapRadiusInTiles; Q > 0; Q--)
	// {
	// 	const FTileCoord FromCoord = FTileCoord(Q,0);
	// 	const FTileCoord ToCoord = FTileCoord(Q-1,0);
	// 	PathRoute.Add(FromCoord, ToCoord);
	// }
	// PathRoute.Add(FTileCoord(), FTileCoord());
	//
	// return PathRoute;
}

TArray<FTileCoord> UAegisMapFactory::GetPathStartCoords(TMap<FTileCoord, FTileCoord> PathRoute) const
{
	// Get all coords where no other tiles points into it
	TSet<FTileCoord> PathStartCoords;
	
	// Get all starting coords
	for (const TTuple<FTileCoord, FTileCoord> Pair : PathRoute)
	{
		PathStartCoords.Add(Pair.Key);
	}
	// Remove all coords which are pointed to
	for (const TTuple<FTileCoord, FTileCoord> Pair : PathRoute)
	{
		PathStartCoords.Remove(Pair.Value);
	}
	return PathStartCoords.Array();
}
