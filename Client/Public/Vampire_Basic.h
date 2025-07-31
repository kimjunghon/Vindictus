#pragma once
#include "Vampire.h"

NS_BEGIN(Client)

class CBody;
class CVampireAI;

//Test
class CPlayerPawn;

class CVampire_Basic final : public CVampire
{
private:
	CVampire_Basic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire_Basic(const CVampire_Basic& Prototype);
	virtual ~CVampire_Basic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual BT_STATE	CanOtherAction() override;
	virtual BT_STATE	CanAttackRange() override;
	virtual BT_STATE	Attack() override;
	virtual BT_STATE	Chase() override;
	virtual BT_STATE	Patrol() override;

private:
	CVampireAI*				m_pAI = { nullptr };
	//Test
	CTransform*				m_pTargetTransform = { nullptr };

private:
	HRESULT Ready_AI();
	HRESULT Ready_PawnObject();
	void	Compute_AnimPosition();

public:
	static CVampire_Basic*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END