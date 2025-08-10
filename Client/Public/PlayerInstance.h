#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CWeapon;
class CArmor;

class CPlayerInstance : public CBase
{
	DECLARE_SINGLETON(CPlayerInstance)

private:
	CPlayerInstance();
	virtual ~CPlayerInstance() = default;

public:
	HRESULT					Initialize(_uint iInventorySlotCount);

public:
	HRESULT					EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon);
	HRESULT					EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor);

	HRESULT					UnEquipWeapon(_uint iWeaponTypeIndex);
	HRESULT					UnEquipArmor(_uint iArmorTypeIndex);

	HRESULT					SavePlayerStatus(const PLAYER_STATUS& PlayerStatus);

	CWeapon*				UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const;
	CArmor*					UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const;
	PLAYER_STATUS			UpdatePlayerStatus() const;

	Shared_ITEM				GetInventory(_uint iInventoryIndex);

	HRESULT					Add_Item(ITEM_TYPE eItemType, CGameObject* pItem);
	HRESULT					Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex);
	_bool					IsInventoryFull() { return m_EmptySlotIndex.empty();}

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CWeapon*				m_pPlayerEquipWeapon[ENUM_CLASS(WEAPON_TYPE::END)] = { nullptr };
	CArmor*					m_pPlayerEquipArmor[ENUM_CLASS(ARMOR_TYPE::END)] = { nullptr };
	
	PLAYER_STATUS			m_PlayerStatus = {};
	
	_uint					m_iInventorySlotCount = {};
	unordered_set<_uint>	m_EmptySlotIndex = {};
	vector<Shared_ITEM>		m_Inventory;

public:
	void			Release_PlayerInstance();
	virtual void	Free() override;
};

NS_END