#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CBody;

class CGlasgavelen final : public CMonster
{
private:
	CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelen(const CGlasgavelen& Prototype);
	virtual ~CGlasgavelen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_PawnObjects();

public:
	static CGlasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END