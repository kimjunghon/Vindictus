#pragma once

#include "StateBar.h"

NS_BEGIN(Client)

class CPlayerStateBar final : public CStateBar
{
private:
	CPlayerStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerStateBar(const CPlayerStateBar& Prototype);
	virtual ~CPlayerStateBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void	Update(_float fTimeDelta) override;

public:
	static CPlayerStateBar*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END