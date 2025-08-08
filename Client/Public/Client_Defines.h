#pragma once

#include "../Default/framework.h"
#include <process.h>

#include "Event_Struct.h"
#include "Client_Struct.h"

#include "../Default/AnimTable.h"
#include "../Default/UITable.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1280;
	const unsigned int			g_iWinSizeY = 720;
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace Client;
