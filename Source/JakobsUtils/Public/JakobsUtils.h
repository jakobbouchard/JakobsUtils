#pragma once

#include "CoreMinimal.h"
#include "JakobsUtils/JakobsUtilsGlobals.h"
#include "Modules/ModuleManager.h"

class FJakobsUtilsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
