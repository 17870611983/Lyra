// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraGameMode.h"

#include "LyraExperienceDefinition.h"
#include "LyraExperienceManagerComponent.h"
#include "LyraGameState.h"
#include "LyraLogChannels.h"
#include "LyraWorldSettings.h"
#include "AssetRegistry/AssetData.h"
#include "Character/LyraPawnData.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"
#include "System/LyraAssetManager.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraGameMode)

ALyraGameMode::ALyraGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass=ALyraGameState::StaticClass();
}


void ALyraGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Wait for the next frame to give time to initialize startup settings
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void ALyraGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;
	FString ExperienceIdSource;

	// Precedence order (highest wins)
	//  - Matchmaking assignment (if present)
	//  - URL Options override
	//  - Developer Settings (PIE only)
	//  - Command Line override
	//  - World Settings
	//  - Dedicated server
	//  - Default experience

	UWorld* World = GetWorld();

	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		//FPrimaryAssetId 类型 和 名字
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
		ExperienceIdSource = TEXT("OptionsString");
	}

	// if (!ExperienceId.IsValid() && World->IsPlayInEditor())
	// {
	// 	ExperienceId = GetDefault<ULyraDeveloperSettings>()->ExperienceOverride;
	// 	ExperienceIdSource = TEXT("DeveloperSettings");
	// }

	// // see if the command line wants to set the experience
	// if (!ExperienceId.IsValid())
	// {
	// 	FString ExperienceFromCommandLine;
	// 	if (FParse::Value(FCommandLine::Get(), TEXT("Experience="), ExperienceFromCommandLine))
	// 	{
	// 		ExperienceId = FPrimaryAssetId::ParseTypeAndName(ExperienceFromCommandLine);
	// 		if (!ExperienceId.PrimaryAssetType.IsValid())
	// 		{
	// 			ExperienceId = FPrimaryAssetId(FPrimaryAssetType(ULyraExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromCommandLine));
	// 		}
	// 		ExperienceIdSource = TEXT("CommandLine");
	// 	}
	// }

	// see if the world settings has a default experience
	if (!ExperienceId.IsValid())
	{
		if (ALyraWorldSettings* TypedWorldSettings = Cast<ALyraWorldSettings>(GetWorldSettings()))
		{
			ExperienceId = TypedWorldSettings->GetDefaultGameplayExperience();
			ExperienceIdSource = TEXT("WorldSettings");
		}
	}

	// ULyraAssetManager& AssetManager = ULyraAssetManager::Get();
	// FAssetData Dummy;
	// if (ExperienceId.IsValid() && !AssetManager.GetPrimaryAssetData(ExperienceId, /*out*/ Dummy))
	// {
	// 	UE_LOG(LogLyraExperience, Error, TEXT("EXPERIENCE: Wanted to use %s but couldn't find it, falling back to the default)"), *ExperienceId.ToString());
	// 	ExperienceId = FPrimaryAssetId();
	// }

	// Final fallback to the default experience
	// if (!ExperienceId.IsValid())
	// {
	// 	if (TryDedicatedServerLogin())
	// 	{
	// 		// This will start to host as a dedicated server
	// 		return;
	// 	}
	//
	// 	//@TODO: Pull this from a config setting or something
	// 	ExperienceId = FPrimaryAssetId(FPrimaryAssetType("LyraExperienceDefinition"), FName("B_LyraDefaultExperience"));
	// 	ExperienceIdSource = TEXT("Default");
	// }

	OnMatchAssignmentGiven(ExperienceId, ExperienceIdSource);
}

void ALyraGameMode::OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId, const FString& ExperienceIdSource)
{
	if (ExperienceId.IsValid())
	{
		UE_LOG(LogLyraExperience, Log, TEXT("Identified experience %s (Source: %s)"), *ExperienceId.ToString(), *ExperienceIdSource);

		//通过GameState获取ULyraExperienceManagerComponent组件
		//因为GameState已经挂载了ULyraExperienceManagerComponent组件
		ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->SetCurrentExperience(ExperienceId);
	}
	else
	{
		UE_LOG(LogLyraExperience, Error, TEXT("Failed to identify experience, loading screen will stay up forever"));
	}
}


const ULyraPawnData* ALyraGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	// if (InController != nullptr)
	// {
	// 	if (const ALyraPlayerState* LyraPS = InController->GetPlayerState<ALyraPlayerState>())
	// 	{
	// 		if (const ULyraPawnData* PawnData = LyraPS->GetPawnData<ULyraPawnData>())
	// 		{
	// 			return PawnData;
	// 		}
	// 	}
	// }

	// If not, fall back to the the default for the current experience
	check(GameState);
	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);

	if (ExperienceComponent->IsExperienceLoaded())
	{
		const ULyraExperienceDefinition* Experience = ExperienceComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData != nullptr)
		{
			return Experience->DefaultPawnData;
		}

		// Experience is loaded and there's still no pawn data, fall back to the default for now
		return ULyraAssetManager::Get().GetDefaultPawnData();
	}

	// Experience not loaded yet, so there is no pawn data to be had
	return nullptr;
}

UClass* ALyraGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const ULyraPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}
