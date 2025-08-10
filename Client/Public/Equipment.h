#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CSlot;
class CPlayerInstance;

class CEquipment final : public CUI_Panel
{
private:
	CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEquipment(const CEquipment& Prototype);
	virtual ~CEquipment() = default;

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

private:
	HRESULT Ready_Children();
	
	
public:
	static CEquipment*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END