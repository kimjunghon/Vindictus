#pragma once
#include "Monster.h"

using namespace Vampire;

NS_BEGIN(Client)

class CVampire abstract : public CMonster
{
protected:
	enum class VAMPIRE_ATTACK	{ ATTACK_NORMAL, END};

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
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) override;

public:
	BT_STATE				Is_Dead();
	BT_STATE				Is_Hit();
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;

protected:
	_uint				m_iStrongFlag = {};

protected:
	HRESULT				Ready_AI();
	void				Compute_AnimPosition();
	void				OnCollisionHit(const CCollider::COLLISION_DATA& CollisionData);
	void				ChangeHitState(ATTACK_TYPE eAttackType, _fvector vHitPosition, _fvector vAttackPosition);
	void				ChangeDeadState(ATTACK_TYPE eAttackType, _fvector vHitPosition, _fvector vAttackPosition);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END