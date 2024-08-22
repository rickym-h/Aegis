// Fill out your copyright notice in the Description page of Project Settings.


#include "AegisMap.h"

#include "MapTile.h"
#include "Aegis/Cards/StructureCard.h"
#include "Aegis/Structures/Structure.h"
#include "Kismet/GameplayStatics.h"
#include "MapTiles/MapTileData.h"


UAegisMap::UAegisMap()
{
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

AMapTile* UAegisMap::CreateMapTile(const FTileCoord Coord, UMapTileData* MapTileData) const
{
	const FVector Location = Coord.ToWorldLocation();
	const FRotator Rotation = FRotator::ZeroRotator;

	AMapTile* Tile = GetWorld()->SpawnActor<AMapTile>(MapTileBP, Location, Rotation);
	Tile->TileCoord = Coord.Copy();

	Tile->SetMapTileData(MapTileData);

	return Tile;
}

TMap<FTileCoord, AMapTile*> UAegisMap::GenerateMapTilesFromData() const
{
	TMap<FTileCoord, AMapTile*> OutMapTiles;

	for (TTuple<FTileCoord, UMapTileData*> Element : MapTileDataMap)
	{
		AMapTile* MapTile = CreateMapTile(Element.Key, Element.Value);
		OutMapTiles.Add(Element.Key, MapTile);
	}

	return OutMapTiles;
}


AStructure* UAegisMap::AddStructureToMap(const UStructureCard* StructureCard, const FTileCoord Location, bool bFinishSpawningStructure)
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

	// Add structure to map and add data about the structure to the structure
	Structure->StructureCard = DuplicateObject(StructureCard, UGameplayStatics::GetGameInstance(this));
	MapStructures.Add(Location, Structure);

	if (bFinishSpawningStructure)
	{
		UGameplayStatics::FinishSpawningActor(Structure, Structure->ActorTransform);
	}
	
	return Structure;
}

bool UAegisMap::IsTileAvailable(const FTileCoord& Location) const
{
	// Check if the coord is even represented by a tile
	if (!MapTiles.Contains(Location)) { return false; }

	// Check the location is not used for an existing structure
	if (MapStructures.Contains(Location)) { return false; }

	// Check the location is not used by an offset of an existing structure
	for (const TTuple<FTileCoord, AStructure*> MapStructurePair : MapStructures)
	{
		const AStructure* Structure = MapStructurePair.Value;
		const FTileCoord StructureLocation = MapStructurePair.Key;
		
		for (const FTileCoord Offset : Structure->StructureCard->StructureOffsets)
		{
			if (Location == StructureLocation + Offset) { return false; }
		}
	}

	return true;
}

bool UAegisMap::CanStructureBePlaced(const UStructureCard* StructureCard, const FTileCoord& Location)
{
	for (const FTileCoord LocalCoord : StructureCard->StructureOffsets)
	{
		const FTileCoord WorldCoord = Location + LocalCoord;
		
		// Check if the tile is available
		if (!IsTileAvailable(WorldCoord)) { return false; }

		// Check that the tile fits the placement constraints of the card
		if (!StructureCard->AllowedTerrains.Contains(MapTileDataMap[WorldCoord]->TerrainType)) { return false; } // Check terrains
		if (!StructureCard->AllowedResources.Contains(MapTileDataMap[WorldCoord]->ResourceType)) { return false; } // Check resources

		// Check that all the map tiles have the same elevations
		if (MapTileDataMap[WorldCoord]->Elevation != MapTileDataMap[Location]->Elevation) { return false; }
	}
	
	return true;
}
