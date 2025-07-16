#pragma once

#pragma warning(disable : 4251)

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <d3dcompiler.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include "Fx11/d3dx11effect.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"
#include "Assimp/scene.h"
#include "Assimp/postprocess.h"
#include "Assimp/Importer.hpp"
#include "Json/json.hpp"
using Json = nlohmann::json;

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
using namespace std;

#include "Engine_Enum.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"

#define UI_FAR 1.f

#define MOVE_LEFT		1
#define MOVE_RIGHT		1<<1
#define MOVE_FRONT		1<<2
#define MOVE_BACK		1<<3

#define ACTION_ATTACK	1
#define ACTION_SMASH	1<<1
#define ACTION_GUARD	1<<2
#define ACTION_DASH		1<<3
#define ACTION_SPRINT	1<<4

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


