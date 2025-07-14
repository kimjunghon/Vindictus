#pragma once

#include "Event.h"
#include "Client_Enum.h"

namespace Client
{
	typedef struct tagEventLevelChagne final : public CEvent
	{
		_uint			iChange_Level;
		_bool			bIsLoading;
	}EVENT_LEVEL_CHANGE;

	typedef struct tagEventProgressBar final : public CEvent
	{
		PROGRESS_TYPE	eType;
		_float			fRatio;
	}EVENT_PROGRESSBAR;

	typedef struct tagEventLoadingComplete final : public CEvent
	{}EVENT_LOADING_COMPLETE;

	typedef struct tagEventOptionSelect final : public CEvent
	{
		_byte byVisibleType;
	}EVENT_UI_CHANGE;
}