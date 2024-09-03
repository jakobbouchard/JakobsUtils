#pragma once

#include "CoreMinimal.h"
#include "JakobsUtilsRuntime/JakobsUtilsGlobals.h"
#include "LatentActions.h"
#include "LatentLerpOverTime.generated.h"

UENUM()
enum class ELatentLerpOverTimeOutputPins : uint8
{
	Update,
	Finished
};

class FLatentLerpOverTime final : public FPendingLatentAction
{
	float TimerCurrentTime = 0.0f;

	bool bShouldExit = false;

public:
	const FLatentActionInfo LatentInfo;

	const float TimerFinalTime = 0.0f;

	const UCurveFloat* Curve;

	ELatentLerpOverTimeOutputPins& Output;

	float& Progress;

	float& Alpha;

	FLatentLerpOverTime(const FLatentActionInfo& LatentInfo, const float Time,
	                    const UCurveFloat* Curve, ELatentLerpOverTimeOutputPins& OutputPins, float& OutProgress,
	                    float& OutAlpha)
		: LatentInfo(LatentInfo),
		  TimerFinalTime(Time),
		  Curve(Curve),
		  Output(OutputPins),
		  Progress(OutProgress),
		  Alpha(OutAlpha)
	{
		Output = ELatentLerpOverTimeOutputPins::Update;
		Progress = 0.0f;
		Alpha = 0.0f;
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

#if WITH_EDITOR
	virtual FString GetDescription() const override
	{
		return FString::Printf(TEXT("Time left: %.1f | Progress: %.1f | Alpha %.1f"), FMath::Max(TimerFinalTime - TimerCurrentTime, 0.0f), Progress, Alpha);
	}
#endif
};
