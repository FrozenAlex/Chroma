#include "Chroma.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
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

MAKE_HOOK_MATCH(MissionLevelScenesTransitionSetupDataSO_Init, &MissionLevelScenesTransitionSetupDataSO::Init, void,
                MissionLevelScenesTransitionSetupDataSO* self, StringW missionId,
                GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
                GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
                ::ArrayW<GlobalNamespace::MissionObjective*> missionObjectives,
                GlobalNamespace::ColorScheme* overrideColorScheme,
                GlobalNamespace::GameplayModifiers* gameplayModifiers,
                GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, StringW backButtonText) {
  MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, difficultyBeatmap, previewBeatmapLevel,
                                               missionObjectives, overrideColorScheme, gameplayModifiers,
                                               playerSpecificSettings, backButtonText);
  SceneTransitionHelper::Patch(difficultyBeatmap);
}

void MissionLevelScenesTransitionSetupDataSOHook(Logger& logger) {
  INSTALL_HOOK(logger, MissionLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MissionLevelScenesTransitionSetupDataSOHook)