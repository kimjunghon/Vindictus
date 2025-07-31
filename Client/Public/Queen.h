#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CBody;

class CQueen final : public CMonster
{
private:
	CQueen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CQueen(const CQueen& Prototype);
	virtual ~CQueen() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_PawnObjects();
	void	Compute_AnimPosition();

public:
	static CQueen*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END