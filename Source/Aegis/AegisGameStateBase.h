// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AegisGameStateBase.generated.h"

class AProjectileManager;
class USaveGame;
class UAegisSaveGame;
class UStructureDataFactory;
class UEnemyFactory;
class UAegisMapFactory;
class UAegisMap;
/**
 * 
 */
UCLASS()
class AEGIS_API AAegisGameStateBase : public AGameStateBase
{
private:
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void GenerateTestMapFromMapFactory();

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Map")
	UAegisMap* AegisMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UAegisMapFactory* MapFactory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UEnemyFactory* EnemyFactory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Factories")
	UStructureDataFactory* StructureDataFactory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile Pool")
	AProjectileManager* ProjectileManager;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UAegisMapFactory> MapFactoryClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UEnemyFactory> EnemyFactoryClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Factory Classes")
	TSubclassOf<UStructureDataFactory> StructureDataFactoryClass;

	UFUNCTION()
	void SaveGameDelegateFunction(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	UFUNCTION()
	void LoadGameDelegateFunction(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
};
