#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

using namespace UI;

class CStorage final : public CUI_Panel
{
private:
	typedef pair<ITEM_TYPE, CGameObject*> ITEM;
private:
	CStorage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStorage(const CStorage& Prototype);
	virtual ~CStorage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_uint			m_iSlotCount = {};
	vector<ITEM>	m_Items;

private:
	HRESULT Ready_Children();
	void	Event_Add_Item(const EVENT_ADD_ITEM& Event);
	HRESULT Add_Item(ITEM_TYPE eItemType, CGameObject* pItem);

public:
	static CStorage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END