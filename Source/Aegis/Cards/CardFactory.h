// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Aegis/Core/Player/PlayerData.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "CardFactory.generated.h"

class UAegisGameInstance;
class UPlayerCard;
/**
 * 
 */
UCLASS(Blueprintable)
class AEGIS_API UCardFactory : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<UPlayerCard*> GenerateStarterTowersForCharacter(UObject* OuterGameInstance, const TEnumAsByte<EPlayerCharacter> Character) const;
	
protected:
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cards")
	// TArray<TSubclassOf<UPlayerCard>> CaiusCards;
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Cards")
	// TArray<TSubclassOf<UPlayerCard>> ValenCards;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Cards to give to the God player (for testing).", RequiredAssetDataTags = "RowStructure=/Script/Aegis.CardData"))
	UDataTable* DefaultKamiCards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Cards to give to the Caius player.", RequiredAssetDataTags = "RowStructure=/Script/Aegis.CardData"))
	UDataTable* DefaultCaiusCards;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Cards to give to the Valen player.", RequiredAssetDataTags = "RowStructure=/Script/Aegis.CardData"))
	UDataTable* DefaultValenCards;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="A list of cards to be selected from for simple card rewards and shop lists.", RequiredAssetDataTags = "RowStructure=/Script/Aegis.CardData"))
	UDataTable* RandomCardSelectionTable;
};

USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY()

	/** Card Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	TSubclassOf<UPlayerCard> CardClass;

	/** Card Value */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Generator")
	int32 BaseGoldValue = 0;
};