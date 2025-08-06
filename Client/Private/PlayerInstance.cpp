#include "ClientPch.h"
#include "PlayerInstance.h"

IMPLEMENT_SINGLETON(CPlayerInstance)

CPlayerInstance::CPlayerInstance()
{
}

HRESULT CPlayerInstance::EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	m_pPlayerEquipWeapon[iWeaponTypeIndex] = pEquipWeapon;
	Safe_AddRef(m_pPlayerEquipWeapon[iWeaponTypeIndex]);

	return S_OK;
}

HRESULT CPlayerInstance::EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;

	m_pPlayerEquipArmor[iArmorTypeIndex] = pEquipArmor;
	Safe_AddRef(m_pPlayerEquipArmor[iArmorTypeIndex]);

	return S_OK;
}

HRESULT CPlayerInstance::UnEquipWeapon(_uint iWeaponTypeIndex)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return E_FAIL;

	Safe_Release(m_pPlayerEquipWeapon[iWeaponTypeIndex]);
	m_pPlayerEquipWeapon[iWeaponTypeIndex] = nullptr;

	return S_OK;
}

HRESULT CPlayerInstance::UnEquipArmor(_uint iArmorTypeIndex)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return E_FAIL;

	Safe_Release(m_pPlayerEquipArmor[iArmorTypeIndex]);
	m_pPlayerEquipArmor[iArmorTypeIndex] = nullptr;

	return S_OK;
}

HRESULT CPlayerInstance::SavePlayerStatus(const PLAYER_STATUS& PlayerStatus)
{
	m_PlayerStatus = PlayerStatus;

	return S_OK;
}

CWeapon* CPlayerInstance::BindPlayerEquipWeapon(_uint iWeaponTypeIndex)
{
	if (iWeaponTypeIndex >= ENUM_CLASS(WEAPON_TYPE::END))
		return nullptr;

	return m_pPlayerEquipWeapon[iWeaponTypeIndex];
}

CArmor* CPlayerInstance::BindPlayerEquipArmor(_uint iArmorTypeIndex)
{
	if (iArmorTypeIndex >= ENUM_CLASS(ARMOR_TYPE::END))
		return nullptr;

	return m_pPlayerEquipArmor[iArmorTypeIndex];
}

PLAYER_STATUS CPlayerInstance::BindPlayerStatus()
{
	return m_PlayerStatus;
}

void CPlayerInstance::Free()
{
	__super::Free();

	for (_uint i = 0; i < ENUM_CLASS(WEAPON_TYPE::END); i++)
		Safe_Release(m_pPlayerEquipWeapon[i]);
	
	for (_uint j = 0; j < ENUM_CLASS(ARMOR_TYPE::END); j++)
		Safe_Release(m_pPlayerEquipArmor[j]);
}
