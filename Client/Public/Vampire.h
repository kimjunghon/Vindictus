#pragma once
#include "Monster.h"

using namespace Vampire;

NS_BEGIN(Client)

class CVampire abstract : public CMonster
{
protected:
	CVampire(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVampire(const CVampire& Prototype);
	virtual ~CVampire() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual BT_STATE		CanAttack();
	virtual BT_STATE		CanOtherAction();
	virtual BT_STATE		CanAttackRange();
	virtual BT_STATE		Attack();
	virtual BT_STATE		Chase();
	virtual BT_STATE		Patrol();

protected:
	_float		m_fAttackTime = {};
	_float		m_fAttackCoolTime = {};
	_float		m_fAttackRange = {};
	_float		m_fChaseRange = {};
	_float		m_fMinDistance = {};


public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END