#include "environment_enhancements/ParametricBoxControllerParameters.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

void Chroma::ParametricBoxControllerParameters::SetTransformScale(ParametricBoxController* parametricBoxController,
                                                                  Sombrero::FastVector3 const& scale) {
  GetParameters(parametricBoxController).Scale = scale;
}

void Chroma::ParametricBoxControllerParameters::SetTransformPosition(
    GlobalNamespace::ParametricBoxController* parametricBoxController, Sombrero::FastVector3 const& position) {
  GetParameters(parametricBoxController).Position = position;
}

Chroma::ParametricBoxControllerParameters& Chroma::ParametricBoxControllerParameters::GetParameters(
    GlobalNamespace::ParametricBoxController* parametricBoxController) {
  return TransformParameters[parametricBoxController];
}
