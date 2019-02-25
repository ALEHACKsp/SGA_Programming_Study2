#pragma once

#include "Framework.h"

#pragma comment(lib, "Framework.lib")

////Fbx SDK
//#define FBXSDK_SHARED
//#include <fbxsdk.h>
//#pragma comment(lib, "libfbxsdk.lib")
//using namespace fbxsdk;

// Assimp
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

#pragma comment(lib, "assimp/assimp-vc140-mt.lib")