// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModularGameMode.h"

#include "LyraGameMode.generated.h"

class AActor;
class AController;
class AGameModeBase;
class APawn;
class APlayerController;
class UClass;
class ULyraExperienceDefinition;
class ULyraPawnData;
class UObject;
struct FFrame;
struct FPrimaryAssetId;
enum class ECommonSessionOnlineMode : uint8;

/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnLyraGameModePlayerInitialized, AGameModeBase* /*GameMode*/,
                                     AController* /*NewPlayer*/);

/**
 * ALyraGameMode
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class ALyraGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	ALyraGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
protected:
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource);
	void HandleMatchAssignmentIfNotExpectingOne();


	UFUNCTION(BlueprintCallable, Category = "Lyra|Pawn")
	const ULyraPawnData* GetPawnDataForController(const AController* InController) const;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

};
