#include "ClientPch.h"
#include "PlayerInstance.h"
#include "Equipment_Manager.h"
#include "Storage_Manager.h"

IMPLEMENT_SINGLETON(CPlayerInstance)

CPlayerInstance::CPlayerInstance()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPlayerInstance::Initialize(_uint iInventorySlotCount)
{
	m_pEquipment_Manager = CEquipment_Manager::Create();
	if (nullptr == m_pEquipment_Manager)
		return E_FAIL;

	m_pStorage_Manager = CStorage_Manager::Create(iInventorySlotCount);
	if (nullptr == m_pStorage_Manager)
		return E_FAIL;

	return S_OK;
}

#pragma region EQUIPMENT_MANAGER
HRESULT CPlayerInstance::EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon, _int iItemInventoryIndex)
{
	return m_pEquipment_Manager->EquipWeapon(iWeaponTypeIndex, pEquipWeapon, iItemInventoryIndex);
}

HRESULT CPlayerInstance::EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor, _int iItemInventoryIndex)
{
	return m_pEquipment_Manager->EquipArmor(iArmorTypeIndex, pEquipArmor, iItemInventoryIndex);
}

HRESULT CPlayerInstance::UnEquipWeapon(_uint iWeaponTypeIndex, _int iItemInventoryIndex)
{
	return m_pEquipment_Manager->UnEquipWeapon(iWeaponTypeIndex, iItemInventoryIndex);
}

HRESULT CPlayerInstance::UnEquipArmor(_uint iArmorTypeIndex, _int iItemInventoryIndex)
{
	return m_pEquipment_Manager->UnEquipArmor(iArmorTypeIndex, iItemInventoryIndex);
}

CWeapon* CPlayerInstance::UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const
{
	return m_pEquipment_Manager->UpdatePlayerEquipWeapon(iWeaponTypeIndex);
}

CArmor* CPlayerInstance::UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const
{
	return m_pEquipment_Manager->UpdatePlayerEquipArmor(iArmorTypeIndex);
}
#pragma endregion

#pragma region STORAGE_MANAGER
Shared_ITEM CPlayerInstance::GetInventory(_uint iInventoryIndex)
{
	return m_pStorage_Manager->GetInventory(iInventoryIndex);
}

HRESULT CPlayerInstance::Add_Item(ITEM_TYPE eItemType, CGameObject* pItem, _int iEmptyInventoryIndex)
{
	return m_pStorage_Manager->Add_Item(eItemType, pItem, iEmptyInventoryIndex);
}

HRESULT CPlayerInstance::Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex)
{
	return m_pStorage_Manager->Swap_Item(iMouseItemIndex, iInventoryIndex);
}

HRESULT CPlayerInstance::Remove_Item(_uint iInventoryIndex)
{
	return m_pStorage_Manager->Remove_Item(iInventoryIndex);
}

_bool CPlayerInstance::IsInventoryFull()
{
	return m_pStorage_Manager->IsInventoryFull();
}
#pragma endregion

HRESULT CPlayerInstance::SavePlayerStatus(const PLAYER_STATUS& PlayerStatus)
{
	m_PlayerStatus = PlayerStatus;

	return S_OK;
}


PLAYER_STATUS CPlayerInstance::UpdatePlayerStatus() const
{
	return m_PlayerStatus;
}

void CPlayerInstance::Release_PlayerInstance()
{
	Release();

	Safe_Release(m_pGameInstance);

	Safe_Release(m_pEquipment_Manager);
	
	Safe_Release(m_pStorage_Manager);
}

void CPlayerInstance::Free()
{
	__super::Free();

}
