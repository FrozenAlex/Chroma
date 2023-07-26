#include "Chroma.hpp"
#include "ChromaController.hpp"

#include <vector>
#include <optional>

#include "UnityEngine/Color.hpp"

#include "Tweening/ColorTween.hpp"

#include "lighting/ChromaLightSwitchEventEffect.hpp"
#include "lighting/ColorTween.hpp"

using namespace Chroma;
using namespace GlobalNamespace;
using namespace UnityEngine;
using namespace System::Collections;
using namespace custom_types::Helpers;
using namespace Tweening;

MAKE_HOOK_MATCH(ColorTween_GetColor, &ColorTween::GetValue, UnityEngine::Color, ColorTween* self, float t) {
  if (!ChromaController::GetChromaLegacy() && !ChromaController::DoChromaHooks()) {
    return ColorTween_GetColor(self, t);
  }

  // discriminator set in ColorTween.makeTween()
  if ((self != nullptr) && self->easeType == Chroma::Tween::ChromaTweenDiscriminator) {
    // Set in ChromaLightSwitchEventEffect
    // fast access
    Chroma::Tween::ChromaColorTweenData const& tweenData =
        *Chroma::ChromaLightSwitchEventEffect::ColorTweensMapping[self];
    return tweenData.GetColor(t);
  }

  return ColorTween_GetColor(self, t);
}

void ColorTweenHook(Logger& logger) {
  INSTALL_HOOK(logger, ColorTween_GetColor);
}

ChromaInstallHooks(ColorTweenHook)