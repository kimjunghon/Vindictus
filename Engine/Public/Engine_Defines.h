#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Fx11/d3dx11effect.h"
#include <d3dcompiler.h>

#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"
#include "Assimp/Importer.hpp"
#include <RapidJson/document.h>
#include <RapidJson/filereadstream.h>
#include <RapidJson/writer.h>
#include <RapidJson/stringbuffer.h>
#include <RapidJson//istreamwrapper.h>
#include "Json/json.hpp"

using Json = nlohmann::json;
using namespace rapidjson;
using namespace DirectX;

#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <ctime>
#include <fstream>
#include <numbers>
#include <filesystem>
using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Binary_Struct.h"

#define UI_FAR 1.f

#define MAX_BONES		512

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif

using namespace Engine;


