// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMap.h"

#include "MapTile.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Structures/Structure.h"
#include "Aegis/Structures/NexusBuilding/NexusBuilding.h"
#include "MapTiles/MapTileData.h"


UAegisMap::UAegisMap()
{
}

void UAegisMap::DestroyMap()
{
	if (NexusBuilding)
	{
		NexusBuilding->Destroy();
	}
	for (const TTuple<FTileCoord, AMapTile*> Element : MapTiles)
	{
		if (AMapTile* Tile = Element.Value)
		{
			Tile->Destroy();
		}
	}
	for (const TTuple<FTileCoord, AStructure*> Element : MapStructures)
	{
		if (AStructure* Structure = Element.Value)
		{
			Structure->Destroy();
		}
	}
}


void UAegisMap::PopulateMapData(
	const TMap<FTileCoord, UMapTileData*>& InMapTileData,
	const TMap<FTileCoord, FTileCoord>& InPathRoute,
	const TArray<FTileCoord>& InPathStartTiles,
	ANexusBuilding* InNexusBuilding)
{
	this->MapTiles = GenerateMapTiles(InMapTileData);
	this->MapTileDataMap = InMapTileData;
	this->PathRoute = InPathRoute;
	this->PathStartTiles = InPathStartTiles;
	this->NexusBuilding = InNexusBuilding;
}

bool UAegisMap::IsCoordInPath(const FTileCoord Coord) const
{
	return PathRoute.Contains(Coord);
}

AMapTile* UAegisMap::GetTile(const FTileCoord Coord)
{
	// For some reason TMap::Find throws an error, so we manually check if is contained and then return the value.
	if (MapTiles.Contains(Coord))
	{
		return MapTiles[Coord];
	}
	return nullptr;
}

TArray<AMapTile*> UAegisMap::GetTiles()
{
	TArray<AMapTile*> Tiles;
	for (const TPair<FTileCoord, AMapTile*>& Entry : MapTiles)
	{
		Tiles.Add(Entry.Value);
	}
	return Tiles;
}

UMapTileData* UAegisMap::GetTileData(FTileCoord Coord)
{
	if (MapTileDataMap.Contains(Coord))
	{
		return MapTileDataMap[Coord];
	}
	return nullptr;
}

FTileCoord UAegisMap::GetEnemySpawnCoord() const
{
	if (PathStartTiles.Num() == 1) { return PathStartTiles[0]; }

	const int RandomIndex = FMath::RandRange(0, PathStartTiles.Num() - 1);
	//UE_LOG(LogTemp, Warning, TEXT("Index: %i"), RandomIndex)
	return PathStartTiles[RandomIndex];
}

FTileCoord UAegisMap::GetNextCoordInPath(const FTileCoord CurrentCoord) const
{
	//return FTileCoord(8, 0);
	return PathRoute[CurrentCoord];
}

int UAegisMap::GetNumOfTilesToEnd(const FTileCoord StartCoord)
{
	int DistanceSoFar = 0;
	FTileCoord CurrentCoord = StartCoord;
	while (PathRoute.Contains(CurrentCoord))
	{
		FTileCoord NextCoord = PathRoute[CurrentCoord];
		if (CurrentCoord == NextCoord) { break; }

		DistanceSoFar++;
		CurrentCoord = NextCoord;
	}
	return DistanceSoFar;
}

AMapTile* UAegisMap::CreateMapTile(const FTileCoord Coord, UMapTileData* MapTileData) const
{
	const FVector Location = Coord.ToWorldLocation();
	const FRotator Rotation = FRotator::ZeroRotator;

	AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(MapTileBP, Location, Rotation);
	Tile->TileCoord = Coord.Copy();

	Tile->SetMapTileData(MapTileData);

	return Tile;
}

TMap<FTileCoord, AMapTile*> UAegisMap::GenerateMapTiles(const TMap<FTileCoord, UMapTileData*>& MapTileData) const
{
	TMap<FTileCoord, AMapTile*> OutMapTiles;

	for (TTuple<FTileCoord, UMapTileData*> Element : MapTileData)
	{
		AMapTile* MapTile = CreateMapTile(Element.Key, Element.Value);
		OutMapTiles.Add(Element.Key, MapTile);
	}

	return OutMapTiles;
}


AStructure* UAegisMap::AddStructureToMap(const UStructureCard* StructureCard, const FTileCoord Location)
{
	if (!StructureCard)
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMap::AddStructureToMap - StructureCard reference is null!"))
		return nullptr;
	}

	if (!CanStructureBePlaced(StructureCard, Location))
	{
		UE_LOG(LogTemp, Error, TEXT("UAegisMap::AddStructureToMap - Structure cannot be placed! Maybe it is overlapping with another structure or the path?"))
		return nullptr;
	}

	// Get the Structure Class
	TSubclassOf<AStructure> StructureBlueprintClass = StructureCard->StructureBlueprintClass;

	// Get target location to spawn structure
	const FTransform ActorTransform = FTransform(GetTile(Location)->StructureLocation);

	// Start spawning the structure. (finish spawning should be handled by the caller)
	UE_LOG(LogTemp, Display, TEXT("UAegisMap::AddStructureToMap - Starting spawning structure..."))
	AStructure* Structure = GetWorld()->SpawnActorDeferred<AStructure>(StructureBlueprintClass, ActorTransform);
	Structure->ActorTransform = ActorTransform;

	return Structure;
}

bool UAegisMap::IsTileAvailable(const FTileCoord Location) const
{
	// Check the location is not part of the path
	if (PathRoute.Contains(Location)) { return false; }

	// Check the location is not used for an existing structure
	if (MapStructures.Contains(Location)) { return false; }

	// Check the location is not used by an offset of an existing structure
	for (TTuple<FTileCoord, AStructure*> MapStructure : MapStructures)
	{
		for (FTileCoord Offset : MapStructure.Value->StructureCard->StructureOffsets)
		{
			const FTileCoord UsedWorldCoord = MapStructure.Key + Offset;
			if (UsedWorldCoord == Location) { return false; }
		}
	}

	return true;
}

bool UAegisMap::CanStructureBePlaced(const UStructureCard* StructureCard, FTileCoord Location)
{
	for (const FTileCoord LocalCoord : StructureCard->StructureOffsets)
	{
		const FTileCoord WorldCoord = Location + LocalCoord;
		if (!IsTileAvailable(WorldCoord)) { return false; }
	}
	return true;
}
