#include "Chroma.hpp"

#include "GlobalNamespace/MissionLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/EnvironmentsListModel.hpp"
#include "GlobalNamespace/BeatmapLevelSO.hpp"

#include "hooks/SceneTransition/SceneTransitionHelper.hpp"

using namespace CustomJSONData;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace Chroma;

MAKE_HOOK_MATCH(
    MissionLevelScenesTransitionSetupDataSO_Init,
    static_cast<void (::GlobalNamespace::MissionLevelScenesTransitionSetupDataSO::*)(
        ::StringW, ByRef<::GlobalNamespace::BeatmapKey>, ::GlobalNamespace::BeatmapLevel*,
        ::ArrayW<::GlobalNamespace::MissionObjective*, ::Array<::GlobalNamespace::MissionObjective*>*>,
        ::GlobalNamespace::ColorScheme*, ::GlobalNamespace::GameplayModifiers*,
        ::GlobalNamespace::PlayerSpecificSettings*, ::GlobalNamespace::EnvironmentsListModel*,
        ::GlobalNamespace::BeatmapLevelsModel*, ::GlobalNamespace::AudioClipAsyncLoader*,
        ::BeatSaber::PerformancePresets::PerformancePreset*, ::GlobalNamespace::BeatmapDataLoader*, ::StringW)>(
        &::GlobalNamespace::MissionLevelScenesTransitionSetupDataSO::Init),
    void, MissionLevelScenesTransitionSetupDataSO* self, ::StringW missionId,
    ByRef<::GlobalNamespace::BeatmapKey> beatmapKey, ::GlobalNamespace::BeatmapLevel* beatmapLevel,
    ::ArrayW<::GlobalNamespace::MissionObjective*, ::Array<::GlobalNamespace::MissionObjective*>*> missionObjectives,
    ::GlobalNamespace::ColorScheme* overrideColorScheme, ::GlobalNamespace::GameplayModifiers* gameplayModifiers,
    ::GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
    ::GlobalNamespace::EnvironmentsListModel* environmentsListModel,
    ::GlobalNamespace::BeatmapLevelsModel* beatmapLevelsModel,
    ::GlobalNamespace::AudioClipAsyncLoader* audioClipAsyncLoader,
    ::BeatSaber::PerformancePresets::PerformancePreset* performancePreset,
    ::GlobalNamespace::BeatmapDataLoader* beatmapDataLoader, ::StringW backButtonText) {
  auto customBeatmapLevel = il2cpp_utils::try_cast<SongCore::SongLoader::CustomBeatmapLevel>(beatmapLevel);
  if (!customBeatmapLevel) {
    MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, beatmapKey, beatmapLevel, missionObjectives,
                                                 overrideColorScheme, gameplayModifiers, playerSpecificSettings,
                                                 environmentsListModel, beatmapLevelsModel, audioClipAsyncLoader,
                                                 performancePreset, beatmapDataLoader, backButtonText);
    return;
  }

  auto environmentInfoBySerializedNameSafe = environmentsListModel->GetEnvironmentInfoBySerializedNameSafe(
      beatmapLevel->GetEnvironmentName(beatmapKey->beatmapCharacteristic, beatmapKey->difficulty)._environmentName);

  // TODO: Fix environment override
  SceneTransitionHelper::Patch(customBeatmapLevel.value(), beatmapKey.heldRef, environmentInfoBySerializedNameSafe);

  MissionLevelScenesTransitionSetupDataSO_Init(self, missionId, beatmapKey, beatmapLevel, missionObjectives,
                                               overrideColorScheme, gameplayModifiers, playerSpecificSettings,
                                               environmentsListModel, beatmapLevelsModel, audioClipAsyncLoader,
                                               performancePreset, beatmapDataLoader, backButtonText);
}

void MissionLevelScenesTransitionSetupDataSOHook() {
  INSTALL_HOOK(ChromaLogger::Logger, MissionLevelScenesTransitionSetupDataSO_Init);
}

ChromaInstallHooks(MissionLevelScenesTransitionSetupDataSOHook)