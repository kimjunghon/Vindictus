#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CGameObject;
NS_END

NS_BEGIN(Client)

class CEquipment_Manager;
class CStorage_Manager;
class CWeapon;
class CArmor;

class CPlayerInstance final : public CBase
{
	DECLARE_SINGLETON(CPlayerInstance)

private:
	CPlayerInstance();
	virtual ~CPlayerInstance() = default;

public:
	HRESULT					Initialize(_uint iInventorySlotCount);

#pragma region EQUIPMENT_MANAGER
public:
	HRESULT					EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon, _int iItemInventoryIndex);
	HRESULT					EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor, _int iItemInventoryIndex);
	HRESULT					UnEquipWeapon(_uint iWeaponTypeIndex, _int iItemInventoryIndex = -1);
	HRESULT					UnEquipArmor(_uint iArmorTypeIndex, _int iItemInventoryIndex= -1);
	CWeapon*				UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const;
	CArmor*					UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const;
#pragma endregion

#pragma region STORAGE_MANAGER
public:
	Shared_ITEM				GetInventory(_uint iInventoryIndex);

	HRESULT					Add_Item(ITEM_TYPE eItemType, CGameObject* pItem, _int iEmptyInventoryIndex = -1);
	HRESULT					Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex);
	HRESULT					Remove_Item(_uint iInventoryIndex);
	_bool					IsInventoryFull();
#pragma endregion

public:
	PLAYER_STATUS			UpdatePlayerStatus() const;
	HRESULT					SavePlayerStatus(const PLAYER_STATUS& PlayerStatus);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	CEquipment_Manager*		m_pEquipment_Manager = { nullptr };
	CStorage_Manager*		m_pStorage_Manager = { nullptr };

	PLAYER_STATUS			m_PlayerStatus = {};

public:
	void			Release_PlayerInstance();
	virtual void	Free() override;
};

NS_END