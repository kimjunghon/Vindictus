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

	if (FAILED(Ready_OriginStatus()))
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
void CPlayerInstance::ChangeStatus(_float fValue, STATUS_TYPE eStatus)
{
	switch (eStatus)
	{
	case STATUS_TYPE::HP:
	{
		m_PlayerStatus.fFullHealth += fValue;
		if(fValue > 0.f)
			m_PlayerStatus.fHealth += fValue;
		else
		{
			if (m_PlayerStatus.fHealth > m_PlayerStatus.fFullHealth)
				m_PlayerStatus.fHealth = m_PlayerStatus.fFullHealth;
		}
		break;
	}
	case STATUS_TYPE::STAMINA:
	{
		m_PlayerStatus.fFullStamina += fValue;
		m_PlayerStatus.fStamina += fValue;

		if (m_PlayerStatus.fStamina <= 0.f)
			m_PlayerStatus.fStamina = 1.f;
		break;
	}

	case STATUS_TYPE::ATK:
		m_PlayerStatus.fAttackDamage += fValue;
		break;

	case STATUS_TYPE::DEF:
		m_PlayerStatus.fDefense += fValue;
		break;
	}

}
void CPlayerInstance::Reset()
{
	m_PlayerStatus.fHealth = m_PlayerStatus.fFullHealth;
	m_PlayerStatus.fStamina = m_PlayerStatus.fFullStamina;

	m_pEquipment_Manager->Reset_Armors();
}
#pragma endregion

HRESULT CPlayerInstance::Ready_OriginStatus()
{
	ifstream File("../Bin/Resources/StatusData/Player_Status.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed PlayerStatus Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("Status") && Doc["Status"].IsObject())
	{
		const Value& Status = Doc["Status"];

		if (Status.HasMember("HP") && Status["HP"].IsFloat())
		{
			m_OriginPlayerStatus.fFullHealth = Status["HP"].GetFloat();
			m_OriginPlayerStatus.fHealth = m_OriginPlayerStatus.fFullHealth;
		}

		if (Status.HasMember("DEF") && Status["DEF"].IsFloat())
			m_OriginPlayerStatus.fDefense = Status["DEF"].GetFloat();

		if (Status.HasMember("ATK") && Status["ATK"].IsFloat())
			m_OriginPlayerStatus.fAttackDamage = Status["ATK"].GetFloat();

		if (Status.HasMember("STAMINA") && Status["STAMINA"].IsFloat())
		{
			m_OriginPlayerStatus.fFullStamina = Status["STAMINA"].GetFloat();
			m_OriginPlayerStatus.fStamina = m_OriginPlayerStatus.fFullStamina;
		}
	}

	m_PlayerStatus = m_OriginPlayerStatus;

	return S_OK;
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
