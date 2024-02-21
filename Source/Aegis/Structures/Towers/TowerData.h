// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Structures/StructureData.h"
#include "UObject/NoExportTypes.h"
#include "TowerData.generated.h"

class ATower;
/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UTowerData : public UStructureData
{
	GENERATED_BODY()

protected:

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tower Data")
	FString DefenderName = "Default Name";


};
