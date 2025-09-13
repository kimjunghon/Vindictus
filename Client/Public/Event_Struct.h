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
	{
		_bool IsBroken;
	}EVENT_BROKEN_HEAD;

	typedef struct tagEventChangeWeapon final : public CEvent
	{
		_uint		iWeaponTypeIndex;
	}EVENT_CHANGE_WEAPON;

	typedef struct tagEventChangeArmor final : public CEvent
	{
		_uint		iArmorTypeIndex;
	}EVENT_CHANGE_ARMOR;
	
	typedef struct tagEventAddItem final : public CEvent
	{
		_uint iInventoryIndex;
	}EVENT_ADD_ITEM;

	typedef struct tagEventPickStorage final : public CEvent
	{
		_uint iInventoryIndex;
	}EVENT_PICK_STORAGE;

	typedef struct tagEventPickEquipment final : public CEvent
	{
		ITEM_TYPE eItemType;
		WEAPON_TYPE eWeaponType;
		ARMOR_TYPE eArmorType;
	}EVENT_PICK_EQUIPMENT;

	typedef struct tagEventUpdateInventory final : public CEvent
	{
		_uint iNumIndices;
		_uint* pIndices = { nullptr };
	}EVENT_UPDATE_INVENTORY;

	typedef struct tagEventPickItem final : public CEvent
	{
		_bool IsPick;
		_uint iInventoryIndex;
	}EVENT_PICK_ITEM;

	typedef struct tagEventPickEquipItem final : public CEvent
	{
		_bool IsPick;
		ITEM_TYPE eItemType;
		WEAPON_TYPE eWeaponType;
		ARMOR_TYPE eArmorType;
	}EVENT_PICK_EQUIP_ITEM;


	typedef struct tagEventNonePick final : public CEvent
	{
	}EVENT_NONE_PICK;

	typedef struct tagEventEquipStorage final : public CEvent
	{
		_uint iInventoryIndex;
	}EVENT_EQUIP_STORAGE;

	typedef struct tagEventUnEquipEquipment final : public CEvent
	{
		ITEM_TYPE eItemType;
		WEAPON_TYPE eWeaponType;
		ARMOR_TYPE eArmorType;
	}EVENT_UNEQUIP_EQUIPMENT;

	typedef struct tagEventBindBossHP final : public CEvent
	{
		_float	m_fLineHP;
		_float	m_fMaxBossHP;
		_float* m_pCurrentBossHP;
	}EVENT_BIND_BOSSHP;
}