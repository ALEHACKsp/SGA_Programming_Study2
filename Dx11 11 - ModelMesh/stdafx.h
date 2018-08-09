#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <functional>
using namespace std;


//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//Json Cpp
#include <json/json.h>
#pragma comment(lib, "jsoncpp.lib")
#pragma warning( disable : 4996 )

//DirectXTex
// 11은 이미지 몇개 안됨 추가 라이브러리 ms에 공개해준거 Texture
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

// FBX SDK
#define FBXSDK_SHARED // 이거 먼저 꼭 해줘야함
#include <fbxsdk.h>
#pragma comment(lib, "libfbxsdk.lib")
using namespace fbxsdk;

const wstring Shaders = L"../_Shaders/";
const wstring Contents = L"../_Contents/";
const wstring Textures = L"../_Textures/";
const wstring Assets = L"../_Assets/";
const wstring Models = L"../_Models/";

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

#include "./Systems/D3D.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/States.h"
#include "./Renders/Shader.h"
#include "./Renders/ShaderBuffer.h"
#include "./Renders/GlobalBuffers.h"
#include "./Renders/Texture.h"
#include "./Renders/Material.h"

#include "./Executes/Execute.h"

#include "./Utilities/Math.h"
#include "./Utilities/Json.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"

typedef VertexTextureNormal ModelVertexType;