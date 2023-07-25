
#pragma once

#include "lighting/LightIDTableManager.hpp"

namespace Chroma {
class TimbalandEnvironment : public EnvironmentData {
public:
  EnvironmentLightDataT getEnvironmentLights() override;
};
} // namespace Chroma
