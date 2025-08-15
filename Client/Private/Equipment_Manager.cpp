#include "ClientPch.h"
#include "Equipment_Manager.h"
#include "Weapon.h"
#include "Armor.h"

CEquipment_Manager::CEquipment_Manager()
	: m_pPlayerInstance { CPlayerInstance::GetInstance() }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEquipment_Manager::EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon, _int iItemInventoryIndex)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	Safe_AddRef(pEquipWeapon);

	m_pPlayerInstance->Remove_Item(iItemInventoryIndex);
	UnEquipWeapon(iWeaponTypeIndex, iItemInventoryIndex);

	m_pPlayerEquipWeapon[iWeaponTypeIndex] = pEquipWeapon;

	EVENT_CHANGE_WEAPON Event = {};
	Event.iWeaponTypeIndex = iWeaponTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

HRESULT CEquipment_Manager::EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor, _int iItemInventoryIndex)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;

	Safe_AddRef(pEquipArmor);

	m_pPlayerInstance->Remove_Item(iItemInventoryIndex);
	UnEquipArmor(iArmorTypeIndex, iItemInventoryIndex);

	m_pPlayerEquipArmor[iArmorTypeIndex] = pEquipArmor;

	EVENT_CHANGE_ARMOR Event = {};
	Event.iArmorTypeIndex = iArmorTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

HRESULT CEquipment_Manager::UnEquipWeapon(_uint iWeaponTypeIndex, _int iItemInventoryIndex)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	if (nullptr == m_pPlayerEquipWeapon[iWeaponTypeIndex])
		return S_OK;

	m_pPlayerInstance->Add_Item(ITEM_TYPE::WEAPON, m_pPlayerEquipWeapon[iWeaponTypeIndex], iItemInventoryIndex);

	m_pPlayerEquipWeapon[iWeaponTypeIndex] = nullptr;

	EVENT_CHANGE_WEAPON Event = {};
	Event.iWeaponTypeIndex = iWeaponTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

HRESULT CEquipment_Manager::UnEquipArmor(_uint iArmorTypeIndex, _int iItemInventoryIndex)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;
	if (nullptr == m_pPlayerEquipArmor[iArmorTypeIndex])
		return S_OK;

	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, m_pPlayerEquipArmor[iArmorTypeIndex], iItemInventoryIndex);

	m_pPlayerEquipArmor[iArmorTypeIndex] = nullptr;

	EVENT_CHANGE_ARMOR Event = {};
	Event.iArmorTypeIndex = iArmorTypeIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::NONSTATIC), Event);

	return S_OK;
}

CWeapon* CEquipment_Manager::UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return nullptr;

	return m_pPlayerEquipWeapon[iWeaponTypeIndex];
}

CArmor* CEquipment_Manager::UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return nullptr;

	return m_pPlayerEquipArmor[iArmorTypeIndex];
}

CEquipment_Manager* CEquipment_Manager::Create()
{
	return new CEquipment_Manager();
}

void CEquipment_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pPlayerInstance);

	for (_uint i = 0; i < ENUM_CLASS(WEAPON_TYPE::END); i++)
		Safe_Release(m_pPlayerEquipWeapon[i]);
	
	for (_uint j = 0; j < ENUM_CLASS(ARMOR_TYPE::END); j++)
		Safe_Release(m_pPlayerEquipArmor[j]);
}
