#include "LatentLerpOverTime.h"

void FLatentLerpOverTime::UpdateOperation(FLatentResponse& Response)
{
	if (bShouldExit)
	{
		Output = ELatentLerpOverTimeOutputPins::Finished;
		Response.FinishAndTriggerIf(true, LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
		return;
	}
	
	TimerCurrentTime += Response.ElapsedTime();
	
	Progress = FMath::Clamp(TimerCurrentTime / TimerFinalTime, 0.0f, 1.0f);
	Alpha = Curve ? Curve->GetFloatValue(Progress) : Progress;
	bShouldExit = Progress == 1.0f;
	
	Response.TriggerLink(LatentInfo.ExecutionFunction, LatentInfo.Linkage, LatentInfo.CallbackTarget);
}
