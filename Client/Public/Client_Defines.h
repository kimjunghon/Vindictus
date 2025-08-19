#pragma once

#include "../Default/framework.h"
#include <process.h>

#include "Event_Struct.h"
#include "Client_Struct.h"

#include "../Default/AnimTable.h"
#include "../Default/UITable.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iInventoryCount = 48;
	//Typedef
	typedef pair<ITEM_TYPE, CGameObject*> ITEM;
	using Shared_ITEM = shared_ptr<ITEM>;

	typedef unordered_map<_uint, vector<ATTACK_MAP>> ATTACK_MAPPING;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;
