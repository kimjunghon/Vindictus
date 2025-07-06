#pragma once
#include "Client_Defines.h"
#include "CollisionObject.h"

NS_BEGIN(Engine)

NS_END

NS_BEGIN(Client)

class CPlayer final : public CCollisionObject
{
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:


private:
	HRESULT Ready_Component();
	

public:
	static CPlayer*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END