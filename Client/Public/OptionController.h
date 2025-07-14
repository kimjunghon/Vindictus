#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

class COptionController final : public CUI_Panel
{
private:
	COptionController(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	COptionController(const COptionController& Prototype);
	virtual ~COptionController() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_byte		m_byVisibleType;
	_wstring	m_strChangeControllerTag = {};

private:
	HRESULT Ready_Children();
	void	Event_UI_Change(const EVENT_UI_CHANGE& Event);

public:
	static COptionController*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

