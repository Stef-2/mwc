module;

#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usd/primDefinition.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

export module mwc_usd;

import mwc_definition;
import mwc_vertex_model;

export namespace usd {
  using pxr::UsdStage;
  using pxr::UsdPrimDefinition;
  using pxr::UsdPrim;
  using pxr::UsdPrimRange;

  using pxr::UsdGeomMesh;
  using pxr::UsdGeomPrimvarsAPI;
  using pxr::UsdShadeMaterialBindingAPI;
  using pxr::VtArray;

  using pxr::GfVec2h;
  using pxr::GfVec2f;
  using pxr::GfVec2d;

  // make sure usd vector types are of the same size as the engine internal ones
  static_assert(sizeof(pxr::GfVec2h) == sizeof(mwc::array_t<mwc::float16_t, 2>));
  static_assert(sizeof(pxr::GfVec2f) == sizeof(mwc::array_t<mwc::float32_t, 2>));
  static_assert(sizeof(pxr::GfVec2d) == sizeof(mwc::array_t<mwc::float64_t, 2>));

  using pxr::GfVec3h;
  using pxr::GfVec3f;
  using pxr::GfVec3d;

  using pxr::GfVec4h;
  using pxr::GfVec4f;
  using pxr::GfVec4d;

  using Matrix3h = pxr::GfMatrixData<pxr::GfHalf, 3, 3>;
  using pxr::GfMatrix3f;
  using pxr::GfMatrix3d;

  using Matrix4h = pxr::GfMatrixData<pxr::GfHalf, 4, 4>;
  using pxr::GfMatrix4f;
  using pxr::GfMatrix4d;

  using pxr::GfQuath;
  using pxr::GfQuatf;
  using pxr::GfQuatd;
}

export namespace mwc {
  template <geometry::vertex_component_c tp_vertex_component>
  constexpr auto usd_vertex_component_map() {}
}