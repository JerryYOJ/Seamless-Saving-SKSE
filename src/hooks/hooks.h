#include <MinHook.h>

#include "SaveOptimization/SaveOptimization.h"

namespace Hooks {
    void Install() { 
        MH_Initialize();

        SaveOptimization::Install();

        MH_EnableHook(MH_ALL_HOOKS);
        return;
    }
}