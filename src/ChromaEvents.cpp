#include "ChromaEvents.hpp"

#include "Chroma.hpp"
#include "lighting/ChromaFogController.hpp"
#include "ChromaComponentManager.hpp"

#include "GlobalNamespace/BeatmapData.hpp"
#include "GlobalNamespace/BeatmapCallbacksController.hpp"

#include "custom-json-data/shared/CustomBeatmapData.h"
#include "custom-json-data/shared/CustomEventData.h"

#include "tracks/shared/TimeSourceHelper.h"
#include "tracks/shared/Vector.h"
#include "tracks/shared/Json.h"
#include <algorithm>

using namespace GlobalNamespace;
using namespace NEVector;

void ChromaEvents::parseEventData(TracksAD::BeatmapAssociatedData& beatmapAD,
                                  CustomJSONData::CustomEventData const* customEventData, bool v2) {
  bool isType = false;

  auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                                                                    \
  static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName);                                        \
  if (!isType && typeHash == (jsonNameHash_##varName)) isType = true;

  TYPE_GET(Chroma::OldConstants::ASSIGNFOGTRACK, ASSIGNFOGTRACK)
  TYPE_GET(Chroma::NewConstants::ANIMATE_COMPONENT, ANIMATE_COMPONENT)

  if (!isType) {
    return;
  }

  rapidjson::Value const& eventData = *customEventData->data;
  auto& eventAD = getEventAD(customEventData);

  if (eventAD.parsed) {
    return;
  }

  eventAD.parsed = true;

  if (typeHash == jsonNameHash_ASSIGNFOGTRACK) {
    auto trackIt = eventData.FindMember((v2 ? Chroma::NewConstants::V2_TRACK : Chroma::NewConstants::TRACK).data());

    if (trackIt == eventData.MemberEnd() || trackIt->value.IsNull() || !trackIt->value.IsString()) {
      getLogger().debug("Track data is missing for Chroma custom event %f", customEventData->time);
      return;
    }

    std::string trackName(trackIt->value.GetString());
    Track* track = beatmapAD.getTrack(trackName);

    eventAD.data.emplace<AssignBloomFogTrack>(track);
  }

  if (typeHash == jsonNameHash_ANIMATE_COMPONENT) {
    auto trackIt = eventData.FindMember((v2 ? Chroma::NewConstants::V2_TRACK : Chroma::NewConstants::TRACK).data());

    if (trackIt == eventData.MemberEnd() || trackIt->value.IsNull() || !trackIt->value.IsString()) {
      getLogger().debug("Track data is missing for Chroma custom event %f", customEventData->time);
      return;
    }

    float duration = NEJSON::ReadOptionalFloat(eventData, Chroma::NewConstants::DURATION.data()).value_or(0);

    auto easing = static_cast<Functions>(NEJSON::ReadOptionalInt(eventData, Chroma::NewConstants::EASING.data())
                                             .value_or(static_cast<int>(Functions::easeLinear)));

    auto tracks = NEJSON::ReadOptionalTracks(eventData, Chroma::NewConstants::TRACK, beatmapAD).value();

    auto const availableNames = { Chroma::NewConstants::BLOOM_FOG_ENVIRONMENT,
                                  Chroma::NewConstants::TUBE_BLOOM_PRE_PASS_LIGHT };
    std::unordered_map<std::string_view, std::vector<AnimateComponentEventData::ComponentData>> coroutineInfos;

    for (auto const& it : eventData[Chroma::NewConstants::COMPONENTS.data()].GetObject()) {
      auto const& componentName = it.name.GetString();
      auto const& component = it.value;
      if (!component.IsObject()) {
        continue;
      }
      if (std::find(availableNames.begin(), availableNames.end(), componentName) == availableNames.end()) {
        continue;
      }

      auto& componentData = coroutineInfos[componentName];
      for (auto const& componentDataIt : component.GetObject()) {
        auto const& propName = componentDataIt.name.GetString();
        auto const& propData = componentDataIt.value.GetArray();
        auto* point = beatmapAD.getPointDefinition(component, propName);

        componentData.emplace_back(propName, point);
      }
    }

    eventAD.data.emplace<AnimateComponentEventData>(duration, easing, tracks, coroutineInfos);
  }
}

void ChromaEvents::deserialize(CustomJSONData::CustomBeatmapData* readOnlyBeatmap) {
  auto* beatmapCast = readOnlyBeatmap;

  auto* beatmap = beatmapCast;
  auto& beatmapAD = TracksAD::getBeatmapAD(beatmap->customData);

  if (!beatmapAD.valid) {
    TracksAD::readBeatmapDataAD(beatmap);
  }

  // Parse events
  for (auto const& customEventData : beatmap->customEventDatas) {
    if (customEventData == nullptr) {
      continue;
    }

    parseEventData(beatmapAD, customEventData, beatmap->v2orEarlier);
  }
}

void CustomEventCallback(BeatmapCallbacksController* callbackController,
                         CustomJSONData::CustomEventData* customEventData) {
  PAPER_IL2CPP_CATCH_HANDLER(
      bool isType = false;

      auto typeHash = customEventData->typeHash;

#define TYPE_GET(jsonName, varName)                                                                                    \
  static auto jsonNameHash_##varName = std::hash<std::string_view>()(jsonName);                                        \
  if (!isType && typeHash == (jsonNameHash_##varName)) isType = true;

      TYPE_GET(Chroma::OldConstants::ASSIGNFOGTRACK, ASSIGNFOGTRACK)
          TYPE_GET(Chroma::NewConstants::ANIMATE_COMPONENT, ANIMATE_COMPONENT)

              if (!isType) { return; }

      auto const& ad = ChromaEvents::getEventAD(customEventData);

      // fail safe, idek why this needs to be done smh
      // CJD you bugger
      auto* customBeatmapData = (CustomJSONData::CustomBeatmapData*)callbackController->beatmapData;
      if (!ad.parsed) {
        TracksAD::BeatmapAssociatedData& beatmapAD = TracksAD::getBeatmapAD(customBeatmapData->customData);
        ChromaEvents::parseEventData(beatmapAD, customEventData, customBeatmapData->v2orEarlier);
      }

      if (typeHash == jsonNameHash_ASSIGNFOGTRACK && customBeatmapData->v2orEarlier) {
        Chroma::ChromaFogController::getInstance()->AssignTrack(
            std::get<ChromaEvents::AssignBloomFogTrack>(ad.data).track);
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Assigned fog controller to track");
      } if (typeHash == jsonNameHash_ANIMATE_COMPONENT && !customBeatmapData->v2orEarlier) {
        Chroma::Component::StartEvent(callbackController, customEventData,
                                      std::get<ChromaEvents::AnimateComponentEventData>(ad.data));
        CJDLogger::Logger.fmtLog<Paper::LogLevel::INF>("Animated component");
      }

  )
}

void ChromaEvents::AddEventCallbacks(Logger& /*logger*/) {
  CustomJSONData::CustomEventCallbacks::AddCustomEventCallback(&CustomEventCallback);
}
