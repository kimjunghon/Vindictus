#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CStorage_Manager final : public CBase
{
private:
	CStorage_Manager();
	virtual ~CStorage_Manager() = default;

public:
	HRESULT					Initialize(_uint iInventorySlotCount);

public:
	Shared_ITEM				GetInventory(_uint iInventoryIndex);

	HRESULT					Add_Item(ITEM_TYPE eItemType, CGameObject* pItem, _int iEmptyInventoryIndex = -1);
	HRESULT					Swap_Item(_uint iMouseItemIndex, _uint iInventoryIndex);
	HRESULT					Remove_Item(_uint iInventoryIndex);
	_bool					IsInventoryFull() { return m_EmptySlotIndex.empty(); }

private:
	CGameInstance*			m_pGameInstance = { nullptr };

	_uint					m_iInventorySlotCount = {};
	set<_uint>				m_EmptySlotIndex = {};
	vector<Shared_ITEM>		m_Inventory;

public:
	static CStorage_Manager*	Create(_uint iInventorySlotCount);
	virtual void				Free();
};

NS_END