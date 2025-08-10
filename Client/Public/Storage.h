#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

using namespace UI;

class CSlot;
class CPlayerInstance;

class CStorage final : public CUI_Panel
{
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
	CPlayerInstance*	m_pPlayerInstance = { nullptr };
	RECT				m_SlotRect = {};
	vector<CSlot*>		m_Slots;
	_uint				m_iPickIndex = {};

	_uint				m_iNumSlotsX = {};
	_uint				m_iNumSlotsY = {};

	_float				m_fSlotStartOffsetX = {};
	_float				m_fSlotStartOffsetY = {};
	_float				m_fSlotOffsetX = {};
	_float				m_fSlotOffsetY = {};

	_float				m_fClickDelay = { };
	_float				m_fCurrentClickDelay = { };

private:
	HRESULT Ready_Children();
	HRESULT Ready_Slots();

	virtual _bool IsPick(HWND hWnd);

	void	Event_Add_Item(const EVENT_ADD_ITEM& Event);
	void	Event_Update_Inventory(const EVENT_UPDATE_INVENTORY& Event);

public:
	static CStorage*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END