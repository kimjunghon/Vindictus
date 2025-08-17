#include "ClientPch.h"
#include "Storage_Manager.h"

CStorage_Manager::CStorage_Manager()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CStorage_Manager::Initialize(_uint iInventorySlotCount)
{
	m_iInventorySlotCount = iInventorySlotCount;
	m_Inventory.resize(iInventorySlotCount, nullptr);

	for (_uint i = 0; i < m_iInventorySlotCount; i++)
		m_EmptySlotIndex.insert(i);

	return S_OK;
}

Shared_ITEM CStorage_Manager::GetInventory(_uint iInventoryIndex)
{
	if (iInventoryIndex >= m_iInventorySlotCount)
		return nullptr;

	return m_Inventory[iInventoryIndex];
}

HRESULT CStorage_Manager::Add_Item(ITEM_TYPE eItemType, CGameObject* pItem, _int iEmptyInventoryIndex)
{
	if (nullptr == pItem)
		return E_FAIL;

	_uint iEmptySlotIndex = {};

	if (iEmptyInventoryIndex == -1)
		iEmptySlotIndex = *m_EmptySlotIndex.begin();
	else
		iEmptySlotIndex = iEmptyInventoryIndex;

	m_Inventory[iEmptySlotIndex] = make_shared<ITEM>(eItemType, pItem);
	m_EmptySlotIndex.erase(iEmptySlotIndex);

	EVENT_UPDATE_INVENTORY Event = {};

	_uint iIndices[1] = { iEmptySlotIndex };
	Event.iNumIndices = 1;
	Event.pIndices = iIndices;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	return S_OK;
}

HRESULT CStorage_Manager::Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex)
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
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	return S_OK;
}

HRESULT CStorage_Manager::Remove_Item(_uint iInventoryIndex)
{
	if (iInventoryIndex >= m_iInventorySlotCount ||
		nullptr == m_Inventory[iInventoryIndex])
		return E_FAIL;

	Safe_Release(m_Inventory[iInventoryIndex]->second);
	m_Inventory[iInventoryIndex] = nullptr;

	m_EmptySlotIndex.insert(iInventoryIndex);

	EVENT_UPDATE_INVENTORY Event = {};

	_uint iIndices[1] = { iInventoryIndex };
	Event.iNumIndices = 1;
	Event.pIndices = iIndices;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	return S_OK;
}

CStorage_Manager* CStorage_Manager::Create(_uint iInventorySlotCount)
{
	CStorage_Manager* pInstance = new CStorage_Manager();
	if (FAILED(pInstance->Initialize(iInventorySlotCount)))
	{
		MSG_BOX(TEXT("Failed Created : CStorage_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStorage_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& pItem : m_Inventory)
	{
		if (pItem)
			Safe_Release(pItem->second);
	}

	m_Inventory.clear();
}
