#include "ClientPch.h"
#include "PlayerInstance.h"
#include "Armor.h"
#include "Weapon.h"

IMPLEMENT_SINGLETON(CPlayerInstance)

CPlayerInstance::CPlayerInstance()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayerInstance::Initialize(_uint iInventorySlotCount)
{
	m_iInventorySlotCount = iInventorySlotCount;
	m_Inventory.resize(iInventorySlotCount, nullptr);
	
	for (_uint i = 0; i < m_iInventorySlotCount; i++)
		m_EmptySlotIndex.insert(i);

	return S_OK;
}

HRESULT CPlayerInstance::EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	if (nullptr != m_pPlayerEquipWeapon[iWeaponTypeIndex])
		Safe_Release(m_pPlayerEquipWeapon[iWeaponTypeIndex]);

	m_pPlayerEquipWeapon[iWeaponTypeIndex] = pEquipWeapon;
	Safe_AddRef(m_pPlayerEquipWeapon[iWeaponTypeIndex]);

	EVENT_CHANGE_WEAPON Event = {};
	Event.iWeaponTypeIndex = iWeaponTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);


	return S_OK;
}

HRESULT CPlayerInstance::EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;

	if (nullptr != m_pPlayerEquipArmor[iArmorTypeIndex])
		Safe_Release(m_pPlayerEquipArmor[iArmorTypeIndex]);

	m_pPlayerEquipArmor[iArmorTypeIndex] = pEquipArmor;
	Safe_AddRef(m_pPlayerEquipArmor[iArmorTypeIndex]);

	EVENT_CHANGE_ARMOR Event = {};
	Event.iArmorTypeIndex = iArmorTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

HRESULT CPlayerInstance::UnEquipWeapon(_uint iWeaponTypeIndex)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	if (nullptr == m_pPlayerEquipWeapon[iWeaponTypeIndex])
		return S_OK;

	Safe_Release(m_pPlayerEquipWeapon[iWeaponTypeIndex]);
	m_pPlayerEquipWeapon[iWeaponTypeIndex] = nullptr;

	EVENT_CHANGE_WEAPON Event = {};
	Event.iWeaponTypeIndex = iWeaponTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

HRESULT CPlayerInstance::UnEquipArmor(_uint iArmorTypeIndex)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;

	if (nullptr == m_pPlayerEquipArmor[iArmorTypeIndex])
		return S_OK;

	Safe_Release(m_pPlayerEquipArmor[iArmorTypeIndex]);
	m_pPlayerEquipArmor[iArmorTypeIndex] = nullptr;

	EVENT_CHANGE_ARMOR Event = {};
	Event.iArmorTypeIndex = iArmorTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);


	return S_OK;
}

HRESULT CPlayerInstance::SavePlayerStatus(const PLAYER_STATUS& PlayerStatus)
{
	m_PlayerStatus = PlayerStatus;

	return S_OK;
}

CWeapon* CPlayerInstance::UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return nullptr;

	return m_pPlayerEquipWeapon[iWeaponTypeIndex];
}

CArmor* CPlayerInstance::UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return nullptr;

	return m_pPlayerEquipArmor[iArmorTypeIndex];
}

PLAYER_STATUS CPlayerInstance::UpdatePlayerStatus() const
{
	return m_PlayerStatus;
}

Shared_ITEM CPlayerInstance::GetInventory(_uint iInventoryIndex)
{
	if (iInventoryIndex >= m_iInventorySlotCount)
		return nullptr;

	return m_Inventory[iInventoryIndex];
}

HRESULT CPlayerInstance::Add_Item(ITEM_TYPE eItemType, CGameObject* pItem)
{
	if (nullptr == pItem)
		return E_FAIL;

	_uint iEmptySlotIndex = *m_EmptySlotIndex.begin();
	m_Inventory[iEmptySlotIndex] = make_shared<ITEM>(eItemType, pItem);

	m_EmptySlotIndex.erase(iEmptySlotIndex);
	
	EVENT_ADD_ITEM Event = {};
	Event.iInventoryIndex = iEmptySlotIndex;

	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);

	return S_OK;
}

HRESULT CPlayerInstance::Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex)
{
	if (nullptr == m_Inventory[iInventoryIndex])
	{
		m_Inventory[iInventoryIndex] = m_Inventory[iMouseItemIndex];
		m_EmptySlotIndex.erase(iInventoryIndex);

		m_Inventory[iMouseItemIndex] = nullptr;
		m_EmptySlotIndex.insert(iMouseItemIndex);
	}
	else
	{
		Shared_ITEM pTemp = m_Inventory[iInventoryIndex];
		m_Inventory[iInventoryIndex] = m_Inventory[iMouseItemIndex];
		m_Inventory[iMouseItemIndex] = pTemp;
	}

	EVENT_UPDATE_INVENTORY Event = {};
	
	_uint iIndices[2] = { iMouseItemIndex, iInventoryIndex };
	Event.iNumIndices = 2;
	Event.pIndices = iIndices;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);

	return S_OK;
}

void CPlayerInstance::Release_PlayerInstance()
{

	Safe_Release(m_pGameInstance);

	for (_uint i = 0; i < ENUM_CLASS(WEAPON_TYPE::END); i++)
		Safe_Release(m_pPlayerEquipWeapon[i]);

	for (_uint j = 0; j < ENUM_CLASS(ARMOR_TYPE::END); j++)
		Safe_Release(m_pPlayerEquipArmor[j]);

	for (auto& pItem : m_Inventory)
	{
		if(pItem)
			Safe_Release(pItem->second);

		int a = 10;
	}

	m_Inventory.clear();
}

void CPlayerInstance::Free()
{
	__super::Free();
}
