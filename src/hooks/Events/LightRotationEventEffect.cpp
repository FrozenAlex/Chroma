#include "Chroma.hpp"
#include "ChromaController.hpp"
#include "utils/ChromaUtils.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/LightRotationEventEffect.hpp"
#include "UnityEngine/Random.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "lighting/ChromaEventData.hpp"
#include "sombrero/shared/RandomUtils.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger,
                &LightRotationEventEffect::HandleBeatmapEvent, void, LightRotationEventEffect* self,
                BasicBeatmapEventData* beatmapEventData) {
  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks()) {
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  auto chromaIt = ChromaEventDataManager::ChromaEventDatas.find(beatmapEventData);

  // Not found
  if (chromaIt == ChromaEventDataManager::ChromaEventDatas.end()) {
    LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger(self, beatmapEventData);
    return;
  }

  auto const& chromaData = chromaIt->second;

  bool isLeftEvent = self->event == BasicBeatmapEventType::Event12;

  bool lockPosition = chromaData.LockPosition;

  float precisionSpeed = chromaData.Speed.value_or(beatmapEventData->value);

  std::optional<int> dir = chromaData.Direction;

  float direction;
  if (dir) {
    switch (dir.value()) {
    case 0:
      direction = isLeftEvent ? -1.0f : 1.0f;
      break;
    case 1:
      direction = isLeftEvent ? 1.0f : -1.0f;
      break;
    }
  } else {
    direction = (ChromaController::randomXoshiro() > 0.5f) ? 1.0f : -1.0f;
  }

  if (beatmapEventData->value == 0) {
    self->set_enabled(false);
    if (!lockPosition) {
      self->get_transform()->set_localRotation(self->startRotation);
    }
  } else if (beatmapEventData->value > 0) {
    self->set_enabled(true);
    self->rotationSpeed = precisionSpeed * 20.0f * direction;
    if (!lockPosition) {
      auto transform = self->get_transform();
      transform->set_localRotation(self->startRotation);
      transform->Rotate(self->rotationVector, ChromaController::randomXoshiro(0.0f, 180.0f), Space::Self);
    }
  }
}

void LightRotationEventEffectHook(Logger& logger) {
  INSTALL_HOOK(logger, LightRotationEventEffect_HandleBeatmapObjectCallbackControllerBeatmapEventDidTrigger);
}

ChromaInstallHooks(LightRotationEventEffectHook)