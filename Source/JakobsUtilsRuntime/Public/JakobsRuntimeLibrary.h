#pragma once

#include "CoreMinimal.h"
#include "JakobsUtilsRuntime/JakobsUtilsGlobals.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JakobsRuntimeLibrary.generated.h"

class UWidget;
enum class ELatentLerpOverTimeOutputPins : uint8;

/**
 * 
 */
UCLASS(DisplayName="Jakob's Runtime Library")
class JAKOBSUTILSRUNTIME_API UJakobsRuntimeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Helper function to know whether we are running in editor.
	 *
	 * @param bIncludeStandalone Will also return true when running in Standalone
	 * @return Whether the world is running in PIE.
	 */
	UFUNCTION(BlueprintPure, Category="Utilities", meta=(WorldContext="WorldContextObject"))
	static bool IsRunningInEditor(const AActor* WorldContextObject, const bool bIncludeStandalone);
	
	/**
	 * Gets the project version from the project settings
	 *
	 * @return The project version or "Unknown"
	 */
	UFUNCTION(BlueprintPure, Category="Utilities")
	static FString GetProjectVersion();

	/**
	 * Lerps from a curve, if provided. If none is provided, it will do a simple linear interpolation from 0 to 1.
	 * 
	 * @param Progress The progress along the curve
	 * @param Alpha The value of the curve at the progress value
	 * @param Time How much time to lerp for
	 * @param Curve The curve to use
	 */
	UFUNCTION(BlueprintCallable, Category="Utilities",
		meta=(WorldContext = "WorldContext", Latent, LatentInfo = "LatentInfo", ExpandEnumAsExecs = "OutputPins", Time = "1.0f"))
	static void LerpOverTime(const UObject* WorldContext, const FLatentActionInfo LatentInfo, const float Time,
							 const UCurveFloat* Curve, ELatentLerpOverTimeOutputPins& OutputPins, float& Progress,
							 float& Alpha);

	/**
	 * Finds the widget that is focused by the provided PlayerController, by looping through all widgets until it finds
	 * it.
	 * @note probably relatively expensive.
	 *
	 * @param OwningPlayer The user's focus to look for.
	 * @return The widget focused by the provided user.
	 */
	UFUNCTION(BlueprintCallable, Category="Widget")
	static UWidget* FindFocusedWidget(APlayerController* OwningPlayer);
	
	//------------ Custom thunk functions ------------//
	
	/** Get a numbered element in the would be generated array of the provided HashMap */
	UFUNCTION(BlueprintCallable, Category="Utilities | Map", CustomThunk,
		meta=(DisplayName = "Get", CompactNodeTitle = "GET", MapParam = "TargetMap", MapKeyParam = "Key", MapValueParam = "Value", AutoCreateRefTerm = "Key, Value", BlueprintThreadSafe))
	static void Map_Get(const TMap<int32, int32>& TargetMap, const int32 Index, int32& Key, int32& Value);
	
	static void GenericMap_Get(void* TargetMap, const FMapProperty* MapProp, int32 Index, void* KeyPtr, void* ValuePtr);
	DECLARE_FUNCTION(execMap_Get)
	{
		// Map
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* MapAddr = Stack.MostRecentPropertyAddress;
		FMapProperty* MapProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
		if (!MapProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		// Index
		P_GET_PROPERTY(FIntProperty, Index);

		// Key
		const FProperty* CurrKeyProp = MapProperty->KeyProp;
		const int32 KeyPropSize = CurrKeyProp->ElementSize * CurrKeyProp->ArrayDim;
		void* KeyStorageSpace = FMemory_Alloca(KeyPropSize);
		CurrKeyProp->InitializeValue(KeyStorageSpace);

		Stack.MostRecentPropertyAddress = NULL;
		Stack.StepCompiledIn<FProperty>(KeyStorageSpace);
		void* KeyPtr = (Stack.MostRecentPropertyAddress != NULL && Stack.MostRecentProperty->GetClass() == CurrKeyProp->GetClass()) ? Stack.MostRecentPropertyAddress : KeyStorageSpace;

		// Value
		const FProperty* CurrValueProp = MapProperty->ValueProp;
		const int32 ValuePropSize = CurrValueProp->ElementSize * CurrValueProp->ArrayDim;
		void* ValueStorageSpace = FMemory_Alloca(ValuePropSize);
		CurrValueProp->InitializeValue(ValueStorageSpace);

		Stack.MostRecentPropertyAddress = NULL;
		Stack.StepCompiledIn<FProperty>(ValueStorageSpace);
		void* ValuePtr = (Stack.MostRecentPropertyAddress != NULL && Stack.MostRecentProperty->GetClass() == CurrValueProp->GetClass()) ? Stack.MostRecentPropertyAddress : ValueStorageSpace;

		P_FINISH;
		
		P_NATIVE_BEGIN;
		GenericMap_Get(MapAddr, MapProperty, Index, KeyPtr, ValuePtr);
		P_NATIVE_END;
		
		CurrValueProp->DestroyValue(ValueStorageSpace);
		CurrKeyProp->DestroyValue(KeyStorageSpace);
	}

	/**
	 * Combine two maps together. Uses a custom K2 node created in JakobsUtilsDeveloper
	 */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly, CustomThunk, meta = (BlueprintThreadSafe, CustomStructureParam = "TargetMap,OtherMap"))
	static void Map_AddOther(TMap<int32, int32> TargetMap, TMap<int32, int32> OtherMap);
	
	static void GenericMap_AddOther(void* TargetMap, void* OtherMap, const FMapProperty* MapProp, const FMapProperty* OtherMapProp);
	DECLARE_FUNCTION(execMap_AddOther)
	{
		// Map
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* MapAddr = Stack.MostRecentPropertyAddress;
		FMapProperty* MapProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
		if (!MapProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		// OtherMap
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* OtherMapAddr = Stack.MostRecentPropertyAddress;
		FMapProperty* OtherMapProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
		if (!OtherMapProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;
		
		P_NATIVE_BEGIN;
		GenericMap_AddOther(MapAddr, OtherMapAddr, MapProperty, OtherMapProperty);
		P_NATIVE_END;
	}
};
