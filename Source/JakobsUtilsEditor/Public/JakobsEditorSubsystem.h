#pragma once

#include "CoreMinimal.h"
#include "JakobsEditorSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogJakobsEditorSubsystem, Warning, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelectionChangedSignature, const TArray<AActor*>&, NewSelection);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSelectedSignature, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDeselectedSignature, AActor*, Actor);

/**
 * 
 */
UCLASS(MinimalAPI, DisplayName="Jakob's Editor Subsystem")
class UJakobsEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

#pragma region Actor Utilities
private:
	UFUNCTION()
	void HandleLevelEditorActorSelectionChanged(const TArray<UObject*>& NewSelection, bool bForceRefresh);
	
	UPROPERTY()
	TArray<AActor*> OldActorSelection;

public:
	/* Event when the selection in the level editor is changed. */
	UPROPERTY(BlueprintAssignable, Category="Editor Scripting | Actor Utility")
	FOnActorSelectionChangedSignature OnActorSelectionChanged;

	/* Event when an actor is selected. */
	UPROPERTY(BlueprintAssignable, Category="Editor Scripting | Actor Utility")
	FOnActorSelectedSignature OnActorSelected;

	/* Event when an actor is deselected. */
	UPROPERTY(BlueprintAssignable, Category="Editor Scripting | Actor Utility")
	FOnActorDeselectedSignature OnActorDeselected;
	
	UFUNCTION(BlueprintCallable, Category="Editor Scripting | Actor Utility", meta=(DeterminesOutputType="ActorClass", DynamicOutputParam="OutActors"))
	static void GetSelectedLevelActorsOfClass(const TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors);
#pragma endregion
};
