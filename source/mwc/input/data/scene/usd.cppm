module;
#define VT_EXPORTS
//#define PXR_USD_USD_SCHEMA_REGISTRY_H
/*
#include <pxr/base/tf/hash.h>
#include <pxr/usd/usd/common.h>
include <pxr/usd/usd/schemaRegistry.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/sdf/path.h>*/
//#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/primDefinition.h>
#include "pxr/base/vt/api.h"

#include "pxr/base/arch/export.h"
#include "pxr/base/arch/defines.h"
#include "pxr/usd/sdf/api.h"

export module mwc_usd;

// import std;

export namespace usd {
  //using pxr::UsdPrim;
  using pxr::UsdPrimDefinition;
  //using pxr::UsdStage;
}