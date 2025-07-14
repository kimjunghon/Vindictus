#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CHUD final : public CUI_Panel
{
private:
	CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CHUD(const CHUD& Prototype);
	virtual ~CHUD() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_byte			m_byVisibleType;

private:
	HRESULT Ready_Children();
	HRESULT Ready_PlayerStateBar();
	void	Event_UI_Change(const EVENT_UI_CHANGE& Event);

public:
	static CHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END