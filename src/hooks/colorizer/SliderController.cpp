#include "Chroma.hpp"
#include "ChromaController.hpp"

#include "colorizer/NoteColorizer.hpp"
#include "colorizer/SliderColorizer.hpp"
#include "custom-json-data/shared/CustomBeatmapData.h"

#include "GlobalNamespace/SliderController.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/SliderMovement.hpp"

#include "utils/ChromaUtils.hpp"

#include "AnimationHelper.hpp"
#include "ChromaObjectData.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace Chroma;
using namespace ChromaUtils;

MAKE_HOOK_MATCH(SliderController_Init, &SliderController::Init, void, SliderController* self,
                ::GlobalNamespace::SliderController::LengthType lengthType, ::GlobalNamespace::SliderData* sliderData,
                float worldRotation, ::UnityEngine::Vector3 headNoteJumpStartPos,
                ::UnityEngine::Vector3 tailNoteJumpStartPos, ::UnityEngine::Vector3 headNoteJumpEndPos,
                ::UnityEngine::Vector3 tailNoteJumpEndPos, float jumpDuration, float startNoteJumpGravity,
                float endNoteJumpGravity, float noteUniformScale) {
  SliderController_Init(self, lengthType, sliderData, worldRotation, headNoteJumpStartPos, tailNoteJumpStartPos,
                        headNoteJumpEndPos, tailNoteJumpEndPos, jumpDuration, startNoteJumpGravity, endNoteJumpGravity,
                        noteUniformScale);

  if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->sliderData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& color = chromaData->second.Color;

    if (color) {
      SliderColorizer::ColorizeSlider(self, *color);
    }
  }
}

MAKE_HOOK_MATCH(SliderController_Update, &SliderController::ManualUpdate, void, SliderController* self) {
  SliderController_Update(self);

  // Do nothing if Chroma shouldn't run
  if (!ChromaController::DoChromaHooks() || !ChromaController::DoColorizerSabers()) {
    return;
  }

  auto chromaData = ChromaObjectDataManager::ChromaObjectDatas.find(self->sliderData);
  if (chromaData != ChromaObjectDataManager::ChromaObjectDatas.end()) {
    auto const& tracks = chromaData->second.Tracks;
    auto pathPointDefinition = chromaData->second.LocalPathColor;
    if (!tracks.empty() || pathPointDefinition) {
      float jumpDuration = self->sliderMovement->jumpDuration;

      float duration = (jumpDuration * 0.75F) + (self->sliderData->tailTime - self->sliderData->time);
      float normalTime = self->sliderMovement->timeSinceHeadNoteJump / (jumpDuration + duration);

      [[maybe_unused]] bool updated = 0;
      std::optional<Sombrero::FastColor> colorOffset =
          AnimationHelper::GetColorOffset(pathPointDefinition, tracks, normalTime, updated, 0);

      if (colorOffset) {

        SliderColorizer::ColorizeSlider(self, colorOffset);
      }
    }
  }
}

void SliderControllerHook() {
  INSTALL_HOOK(ChromaLogger::Logger, SliderController_Init);
  INSTALL_HOOK(ChromaLogger::Logger, SliderController_Update);
}

ChromaInstallHooks(SliderControllerHook)