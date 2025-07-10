#pragma once
#include "StateBar.h"

NS_BEGIN(Client)

class CStateBar_Lerp final : public CStateBar
{
private:
	CStateBar_Lerp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CStateBar_Lerp(const CStateBar_Lerp& Prototype);
	virtual ~CStateBar_Lerp() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void	Update(_float fTimeDelta) override;

public:
	static CStateBar_Lerp*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END