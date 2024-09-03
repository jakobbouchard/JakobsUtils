#include "JakobsSteamSubsystem.h"

void UJakobsSteamSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	if (!GetWorld()->IsGameWorld()) return;
	if (!SteamAPI_Init()) return;

	SteamCallbackHelper = MakeUnique<FSteamCallbackHelper>();
	SteamCallbackHelper->OnGameOverlayActivated.BindUObject(this, &UJakobsSteamSubsystem::BroadcastGameOverlayActivated);
}

bool UJakobsSteamSubsystem::IsRunningOnSteamDeck()
{
	if (!SteamAPI_Init()) return false;
	
	return SteamUtils()->IsSteamRunningOnSteamDeck();
}

bool UJakobsSteamSubsystem::IsGameOverlayActivated() const
{
	if (!SteamCallbackHelper.IsValid()) return false;
	
	return SteamCallbackHelper->bGameOverlayActivated;
}

void UJakobsSteamSubsystem::BroadcastGameOverlayActivated()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnGameOverlayActivated.Broadcast(IsGameOverlayActivated());
	});
}

void FSteamCallbackHelper::GameOverlayActivatedCallback(GameOverlayActivated_t* pParam)
{
	bGameOverlayActivated = static_cast<bool>(pParam->m_bActive);
	(void)OnGameOverlayActivated.ExecuteIfBound();
}
