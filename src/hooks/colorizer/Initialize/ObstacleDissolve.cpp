#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "colorizer/ObstacleColorizer.hpp"

#include <vector>
#include <optional>

#include "custom-json-data/shared/CustomBeatmapData.h"

#include "custom-types/shared/types.hpp"
#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/register.hpp"
#include "custom-types/shared/coroutine.hpp"

#include "GlobalNamespace/ObstacleDissolve.hpp"

#include "UnityEngine/Color.hpp"
#include "UnityEngine/WaitForEndOfFrame.hpp"

#include <experimental/coroutine>

using namespace CustomJSONData;
using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;

MAKE_HOOK_MATCH(ObstacleDissolve_Awake, &ObstacleDissolve::Awake, void, ObstacleDissolve* self) {
  ObstacleDissolve_Awake(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }
  ObstacleColorizer::New(self->obstacleController);
}

MAKE_HOOK_MATCH(ObstacleDissolve_OnDestroy, &ObstacleDissolve::OnDestroy, void, ObstacleDissolve* self) {
  ObstacleDissolve_OnDestroy(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    return;
  }

  ObstacleColorizer::Colorizers.erase(self->obstacleController);
}

void ObstacleDissolveHook(Logger& logger) {
  INSTALL_HOOK(logger, ObstacleDissolve_Awake);
  INSTALL_HOOK(logger, ObstacleDissolve_OnDestroy);
}

ChromaInstallHooks(ObstacleDissolveHook)