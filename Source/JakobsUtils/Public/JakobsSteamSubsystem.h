// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JakobsUtils/JakobsUtilsGlobals.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "steam/steam_api.h"
#include "JakobsSteamSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOverlayActivated, bool, bOpen);

/**
 * A subsystem that relays some events from Steamworks.
 */
UCLASS(DisplayName="Jakob's Steam Subsystem")
class JAKOBSUTILS_API UJakobsSteamSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/**
	 * @return True if running on Steam Deck
	 */
	UFUNCTION(BlueprintPure, Category = "Utilities|Steamworks")
	static bool IsRunningOnSteamDeck();
	
	/**
	 * @return True if the overlay is currently open
	 */
	UFUNCTION(BlueprintPure, Category = "Utilities|Steamworks")
	bool IsGameOverlayActivated() const;
	
	/**
	 * Fires when the overlay opens and closes.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Utilities|Steamworks")
	FOnGameOverlayActivated OnGameOverlayActivated;

private:
	TUniquePtr<class FSteamCallbackHelper> SteamCallbackHelper;
	
	void BroadcastGameOverlayActivated();
};

class FSteamCallbackHelper
{
public:
	FSteamCallbackHelper():
	m_CallbackGameOverlayActivated(this, &FSteamCallbackHelper::GameOverlayActivatedCallback)
	{};
		
	STEAM_CALLBACK(FSteamCallbackHelper, GameOverlayActivatedCallback, GameOverlayActivated_t, m_CallbackGameOverlayActivated);

	FSimpleDelegate OnGameOverlayActivated;

	bool bGameOverlayActivated = false;
};