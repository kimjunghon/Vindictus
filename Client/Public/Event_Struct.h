#pragma once

#include "Event.h"
#include "Client_Enum.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

namespace Client
{
	typedef struct tagEventLevelChagne final : public CEvent
	{
		_uint			iChange_Level;
		_bool			bIsLoading;
	}EVENT_LEVEL_CHANGE;

	typedef struct tagEventUILevelChange final : public CEvent
	{
		_uint			iChange_Level;
		_bool			bIsLoading;
	}EVENT_UI_LEVEL_CHANGE;

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

	typedef struct tagEventBrokenHead final : public CEvent
	{}EVENT_BROKEN_HEAD;

	typedef struct tagEventAddItem final : public CEvent
	{
		ITEM_TYPE eItemType;
		CGameObject* pItem;
	}EVENT_ADD_ITEM;

	typedef struct tagEventChangeWeapon final : public CEvent
	{
		_uint		iWeaponTypeIndex;
	}EVENT_CHANGE_WEAPON;

	typedef struct tagEventChangeArmor final : public CEvent
	{
		_uint		iArmorTypeIndex;
	}EVENT_CHANGE_ARMOR;

}