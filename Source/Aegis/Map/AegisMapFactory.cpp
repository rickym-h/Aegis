// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMapFactory.h"

#include "IContentBrowserSingleton.h"
#include "MapTile.h"
#include "PathGenerationBlueprintLibrary.h"
#include "Aegis/Structures/NexusBuilding.h"
#include "Kismet/BlueprintPathsLibrary.h"

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

UAegisMap* UAegisMapFactory::GenerateMap(const int PathClusterLength, const int PathsCount, const int BranchesCount) const
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
	const TMap<FTileCoord, FTileCoord> PathClusters = GeneratePath(PathClusterLength, PathsCount, BranchesCount);
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

UAegisMap* UAegisMapFactory::GenerateMapWithNoise(const int MainPathLength) const
{
	
	FRandomStream RandomStream = FRandomStream(0);
	const FVector2D ElevationNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	const FVector2D ResourceNoiseOffset = FVector2D(RandomStream.FRandRange(-100000.f, 100000.f), RandomStream.FRandRange(-100000.f, 100000.f));
	
	// Get the poisson sampled node points using the blue noise algorithm
	TMap<FTileCoord, FTileCoord> Path = UPathGenerationBlueprintLibrary::GenerateGreedyPoissonPath(MainPathLength, ElevationNoiseOffset, RandomStream);
	

	TArray<float> NoiseDistribution;
	TMap<FTileCoord, AMapTile*> MapTiles;
	TArray<FTileCoord> PathTiles;
	for (TTuple<FTileCoord, FTileCoord> Elem : Path)
	{
		PathTiles.Add(Elem.Key);
	}
	for (const FTileCoord Coord : FTileCoord::GetTilesInRadius(PathTiles, 10))
	{
		//if (Path.Contains(Coord)) { continue; }
		FVector Location = Coord.ToWorldLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		
		FActorSpawnParameters SpawnInfo;
	
		// Spawn actual tile BP in world
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(GrassTileBP, Location, Rotation);
		Tile->TileCoord = Coord.Copy();

		float Gradient = (UPathGenerationBlueprintLibrary::GetNodeWeight(Coord, ElevationNoiseOffset, false) + 1.f)/2.f;
		Tile->PathingGradient = Gradient;
		NoiseDistribution.Emplace(Gradient);
		
		Tile->ToggleShowGradients();

		MapTiles.Add(Coord, Tile);
	}

	NoiseDistribution.Sort();
	// Water	10%
	float WaterLimit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.1)];
	// Grass 1	50%
	float Grass1Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.9)];
	// Grass 2	10%
	float Grass2Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.95)];
	// Grass 3	10%
	float Grass3Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.97)];
	// Grass 4	10%
	float Grass4Limit = NoiseDistribution[static_cast<int>((NoiseDistribution.Num()-1) * 0.99)];
	// Stone 5	10%
	//float StoneLimit = NoiseDistribution[static_cast<int>(NoiseDistribution.Num() * 1)];
	
	for (TTuple<FTileCoord, AMapTile*> Elem : MapTiles)
	{
		if (PathTiles.Contains(Elem.Key))
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 0.75));
			continue;
		}
		Elem.Value->ToggleShowGradients();
		if (Elem.Value->PathingGradient < WaterLimit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 0.5));
		} else if (Elem.Value->PathingGradient < Grass1Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 1));
		} else if (Elem.Value->PathingGradient < Grass2Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 1.5));
		} else if (Elem.Value->PathingGradient < Grass3Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 2));
		} else if (Elem.Value->PathingGradient < Grass4Limit)
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 2.5));
		} else
		{
			Elem.Value->TileMesh->SetWorldScale3D(FVector(1, 1, 3));
		}
	}

	
	// Add nexus building
	ANexusBuilding* NexusBuilding = GetWorld()->SpawnActor<ANexusBuilding>(NexusBuildingBP, FVector(0,0,0), FRotator(0,0,0));

	// Set path start tile coords
	const TArray<FTileCoord> PathStartTileCoords = GetPathStartCoords(Path);

	// Generate tiles - setting tile heights, and tile types

	// Create Map instance
	// Create Map
	UAegisMap* Map = NewObject<UAegisMap>(GetWorld(), AegisMapClass);
	Map->PopulateMapData(MapTiles, Path, PathStartTileCoords, NexusBuilding);

	// Set TilesToEnd of every tile
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
	
	// Generate tiles near each path cluster
	for (TTuple<FTileCoord, FTileCoord> Entry : PathClusters)
	{
		TArray<FTileCoord> TilesInRadius = FTileCoord::GetTilesInRadius(Entry.Key, 2);
		for (FTileCoord TileCoord : TilesInRadius)
		{
			CoordsInRangeOfPath.Add(TileCoord);
		}

	}
	
	UE_LOG(LogTemp, Warning, TEXT("UAegisMapFactory::GenerateMapTiles() - Generating %d tiles."), CoordsInRangeOfPath.Num())
	
	TMap<FTileCoord, AMapTile*> MapTiles;
	for (const FTileCoord ThisTileCoord : CoordsInRangeOfPath.Array())
	{		
		// World location of tile BP
		FVector Location = ThisTileCoord.ToWorldLocation();
		FRotator Rotation(0.0f, 0.0f, 0.0f);
			
		FActorSpawnParameters SpawnInfo;

		// TODO decide if it should be a GrassTileBP or a PathTile
		// (for now it is just visualised by toggling debug gradients at end of tile creation)

		// Spawn actual tile BP in world
		AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(GrassTileBP, Location, Rotation);

		FVector2D NoiseSampleLoc = FVector2D(Location.X/1000, Location.Y/1000) + RandomNoiseOffset;
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
	// TODO check constrains for other inputs

	// Set Branching Points
	TArray<int> BranchingLengths;
	float BranchingSpacing = static_cast<float>(PathLengthInClusters - PathsCount) / static_cast<float>(BranchesCount + 1);
	UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - Spacing: %f"), BranchingSpacing)
	for (float i = 1; i <= BranchesCount; i++)
	{
		int BranchPoint = static_cast<int>(i*BranchingSpacing);
		BranchingLengths.Add(BranchPoint + PathsCount + 1);
		UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - BranchPoint: %d"), BranchPoint)
	}
	
	
	// Init a stack of Paths (stack for DFS, queue for BFS)
	TArray<TMap<FTileCoord,FTileCoord>> PossiblePaths;

	// Add the first paths onto the stack
	// TODO add every possible first combination of N Paths instead of just the first one
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
		//UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - %d - Testing path of length: %d"), Count, Path.Num())

		// If the path is in the goal state, return it.
		if (UPathGenerationBlueprintLibrary::PathMapIsValid(Path, PathLengthInClusters, PathsCount, BranchesCount))
		{
			return Path;
		}

		// Enumerate all possible next states, and add them to the stack
		if (Path.Num()-1 > PathLengthInClusters) { continue; }

		FTileCoord NodeToBranchFrom;
		if (BranchingLengths.Contains(Path.Num()))
		{
			NodeToBranchFrom = UPathGenerationBlueprintLibrary::GetClusterToBranchFrom(Path);
			UE_LOG(LogTemp, Error, TEXT("UAegisMapFactory::GeneratePath() - Attempting to branch from %ls"), *NodeToBranchFrom.ToString())
		} else
		{
			// Get the node to generate the next path from (shortest distance to centre)
			NodeToBranchFrom = UPathGenerationBlueprintLibrary::GetClosestEndToCentre(Path);
		}

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
