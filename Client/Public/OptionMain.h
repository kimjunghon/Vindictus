#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

class COptionMain final : public CUI_Panel
{
private:
	COptionMain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	COptionMain(const COptionMain& Prototype);
	virtual ~COptionMain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_byte		m_byVisibleType = {};
private:
	HRESULT Ready_Children();
	void	Event_UI_Change(const EVENT_UI_CHANGE& Event);

public:
	static COptionMain*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

