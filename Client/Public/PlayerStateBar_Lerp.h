#pragma once
#include "StateBar.h"

NS_BEGIN(Client)

class CPlayerStateBar_Lerp final : public CStateBar
{
private:
	CPlayerStateBar_Lerp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayerStateBar_Lerp(const CPlayerStateBar_Lerp& Prototype);
	virtual ~CPlayerStateBar_Lerp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void	Update(_float fTimeDelta) override;

public:
	static CPlayerStateBar_Lerp*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END