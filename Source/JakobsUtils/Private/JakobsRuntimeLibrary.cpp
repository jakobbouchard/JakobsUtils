#include "JakobsRuntimeLibrary.h"

#include "GeneralProjectSettings.h"
#include "LatentLerpOverTime.h"
#include "Components/Widget.h"


#define LOCTEXT_NAMESPACE "JakobsRuntimeLibrary"

bool UJakobsRuntimeLibrary::IsRunningInEditor(const AActor* WorldContextObject, const bool bIncludeStandalone)
{
#ifdef WITH_EDITOR
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		return bIncludeStandalone ? true : World->WorldType == EWorldType::PIE;
	}
#endif
	return false;
}

FString UJakobsRuntimeLibrary::GetProjectVersion()
{
	if (const UGeneralProjectSettings* Settings = GetDefault<UGeneralProjectSettings>())
	{
		return Settings->ProjectVersion;
	}

	return TEXT("Unknown");
}

void UJakobsRuntimeLibrary::LerpOverTime(const UObject* WorldContext, const FLatentActionInfo LatentInfo,
                                       const float Time, const UCurveFloat* Curve,
                                       ELatentLerpOverTimeOutputPins& OutputPins, float& Progress, float& Alpha)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);

	if (!World)
	{
		UE_LOG(LogJakobsUtils, Error, TEXT("Could not create LerpOverTime: No World found."))
		return;
	}

	FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

	if (LatentActionManager.FindExistingAction<FLatentLerpOverTime>(LatentInfo.CallbackTarget, LatentInfo.UUID))
	{
		UE_LOG(LogJakobsUtils, Warning, TEXT("Tried to re-execute a LerpOverTime that was already started"))
		return;
	}

	FLatentLerpOverTime* Action = new FLatentLerpOverTime(LatentInfo, Time, Curve, OutputPins, Progress, Alpha);
	LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
}

UWidget* UJakobsRuntimeLibrary::FindFocusedWidget(APlayerController* OwningPlayer)
{
	for (TObjectIterator<UWidget> It; It; ++It) {
		if (It->HasUserFocus(OwningPlayer)) return *It;
	}
	return nullptr;
}

//------------ Custom thunk functions ------------//

void UJakobsRuntimeLibrary::Map_Get(const TMap<int32, int32>& TargetMap, const int32 Index, int32& Key, int32& Value)
{
	checkNoEntry();
}

void UJakobsRuntimeLibrary::GenericMap_Get(void* TargetMap, const FMapProperty* MapProp, int32 Index, void* KeyPtr,
											void* ValuePtr)
{
	if (TargetMap)
	{
		FScriptMapHelper MapHelper(MapProp, TargetMap);

		FProperty* KeyProp = MapHelper.KeyProp;
		FProperty* ValueProp = MapHelper.ValueProp;

		if (MapHelper.IsValidIndex(Index))
		{
			KeyProp->CopyCompleteValueFromScriptVM(KeyPtr, MapHelper.GetKeyPtr(Index));
			ValueProp->CopyCompleteValueFromScriptVM(ValuePtr, MapHelper.GetValuePtr(Index));
		} else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Attempted to access index %d from map '%s' of length %d in '%s'!"),
				Index,
				*MapProp->GetName(),
				MapHelper.Num(),
				*MapProp->GetOwnerVariant().GetPathName()),
				ELogVerbosity::Warning,
				FName("GetOutOfBoundsWarning"));
			KeyProp->InitializeValue(KeyPtr);
			ValueProp->InitializeValue(ValuePtr);
		}
	}
}

void UJakobsRuntimeLibrary::Map_AddOther(TMap<int32, int32> TargetMap, TMap<int32, int32> OtherMap)
{
	checkNoEntry();
}

void UJakobsRuntimeLibrary::GenericMap_AddOther(void* TargetMap, void* OtherMap, const FMapProperty* MapProp, const FMapProperty* OtherMapProp)
{
	if (TargetMap && OtherMap)
	{
		FScriptMapHelper MapHelper(MapProp, TargetMap);
		FScriptMapHelper OtherMapHelper(OtherMapProp, OtherMap);

		for (int32 Index = 0; Index < OtherMapHelper.GetMaxIndex(); ++Index)
		{
			if (OtherMapHelper.IsValidIndex(Index))
			{
				MapHelper.AddPair(OtherMapHelper.GetKeyPtr(Index), OtherMapHelper.GetValuePtr(Index));
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE	// "JakobsRuntimeLibrary"
