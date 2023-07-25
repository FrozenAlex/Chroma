#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/NoteController.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"
#include "colorizer/NoteColorizer.hpp"
#include "UnityEngine/Space.hpp"
#include "UnityEngine/Transform.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init,
                void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode,
                GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
                GlobalNamespace::BeatmapDifficulty beatmapDifficulty,
                GlobalNamespace::BeatmapCharacteristicSO* beatmapCharacteristic,
                GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
                GlobalNamespace::ColorScheme* overrideColorScheme,
                GlobalNamespace::GameplayModifiers* gameplayModifiers,
                GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
                GlobalNamespace::PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects) {
  MultiplayerLevelScenesTransitionSetupDataSO_Init(
      self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap,
      overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
  SceneTransitionHelper::Patch(difficultyBeatmap);
}

void MultiplayerLevelScenesTransitionSetupDataSOHook(Logger& logger) {
  INSTALL_HOOK(logger, MultiplayerLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MultiplayerLevelScenesTransitionSetupDataSOHook)