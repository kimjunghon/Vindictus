#pragma once
#include "Slot.h"

NS_BEGIN(Client)

class CStorageSlot : public CSlot
{
public:
	typedef struct tagStorageSlotDesc : public UIOBJECT_DESC {
		_uint iInventoryIndex;
	}STORAGE_SLOT_DESC;

private:
	CStorageSlot(ID3D11Device* pDevice, ID3D11Device* pDeviceContext);
	CStorageSlot(const CStorageSlot& Prototype);
	virtual ~CStorageSlot() = default;

public:
	const _uint		Get_InventoryInedx() const { return m_iInventoryIndex; }
	void			Update_ItemData(ITEM* pItem) { m_pItem = pItem; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint		m_iInventoryIndex = {};

public:
	static CStorageSlot*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END