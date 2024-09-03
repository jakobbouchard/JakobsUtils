#include "JakobsEditorSubsystem.h"

#include "EditorScriptingHelpers.h"
#include "LevelEditor.h"
#include "Selection.h"

#define LOCTEXT_NAMESPACE "JakobsEditorSubsystem"

DEFINE_LOG_CATEGORY(LogJakobsEditorSubsystem);

namespace InternalJakobsEditorSubsystemLibrary
{
	template<class T>
	bool IsEditorLevelActor(T* Actor)
	{
		bool bResult = false;
		if (Actor && IsValidChecked(Actor))
		{
			const UWorld* World = Actor->GetWorld();
			if (World && World->WorldType == EWorldType::Editor)
			{
				bResult = true;
			}
		}
		return bResult;
	}
}

void UJakobsEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#pragma region Actor Utilities
	FLevelEditorModule& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditor.OnActorSelectionChanged().AddUObject(this, &UJakobsEditorSubsystem::HandleLevelEditorActorSelectionChanged);
#pragma endregion 
}

void UJakobsEditorSubsystem::Deinitialize()
{
	Super::Deinitialize();

#pragma region Actor Utilities
	FLevelEditorModule& LevelEditor = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
	LevelEditor.OnActorSelectionChanged().RemoveAll(this);
#pragma endregion 
}

#pragma region Actor Utilities
void UJakobsEditorSubsystem::HandleLevelEditorActorSelectionChanged(const TArray<UObject*>& NewSelection,
	bool bForceRefresh)
{
	TArray<AActor*> NewActors;
	for (const auto Object : NewSelection)
	{
		if (auto Actor = Cast<AActor>(Object))
		{
			NewActors.Add(Actor);
		}
	}
	OnActorSelectionChanged.Broadcast(NewActors);

	for (const auto Actor : NewActors) {
		bool bFound = false;
		for (const auto OldActor : OldActorSelection) {
			if (Actor == OldActor) {
				bFound = true;
				OldActorSelection.Remove(Actor);
				break;
			}
		}
		if (!bFound) {
			OnActorSelected.Broadcast(Actor);
		}
	}

	// if there still are old objects they are no longer selected so broadcast it
	if (OldActorSelection.Num() > 0) {
		for (const auto Actor : OldActorSelection) {
			OnActorDeselected.Broadcast(Actor);
		}
	}

	OldActorSelection = NewActors;
}

void UJakobsEditorSubsystem::GetSelectedLevelActorsOfClass(const TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors)
{
	TGuardValue<bool> UnattendedScriptGuard(GIsRunningUnattendedScript, true);

	if (!EditorScriptingHelpers::CheckIfInEditorAndPIE())
	{
		return;
	}

	for (FSelectionIterator Iter(*GEditor->GetSelectedActors()); Iter; ++Iter)
	{
		AActor* Actor = Cast<AActor>(*Iter);
		if (!InternalJakobsEditorSubsystemLibrary::IsEditorLevelActor(Actor))
		{
			return;
		}
		if (!Actor->GetClass()->IsChildOf(ActorClass))
		{
			return;
		}
		OutActors.Add(Actor);	
	}
}
#pragma endregion 

#undef LOCTEXT_NAMESPACE	// "JakobsEditorSubsystem"
