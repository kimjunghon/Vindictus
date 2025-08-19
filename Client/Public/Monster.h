#pragma once
#include "ColliderPawn.h"

NS_BEGIN(Engine)
class CBehaviorTree;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)

class CBody;

class CMonster abstract : public CColliderPawn
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) PURE;

	//State
public:
	virtual BT_STATE		CanAttack();
	virtual BT_STATE		CanOtherAction();
	virtual BT_STATE		CanAttackRange();
	virtual BT_STATE		Attack() PURE;
	virtual BT_STATE		Chase() PURE;
	virtual BT_STATE		Patrol() PURE;

protected:
	_bool			m_AttackComplete = { false };
	_uint			m_iNumAttacks = {};
	vector<_float>	m_AttackTime = {};
	vector<_float>	m_AttackCoolTime = {};

	_uint			m_iCurrentAttack = {};
	
	_float			m_fAttackRange = {};
	_float			m_fChaseRange = {};
	_float			m_fMinDistance = {};
	//Test
	CTransform*		m_pTargetTransform = { nullptr };

protected:
	CBehaviorTree*		m_pAI = { nullptr };
	CBody*				m_pBody = { nullptr };
	const _vector*		m_pAnimMovement = {nullptr};
	const _vector*		m_pAnimRotation = { nullptr };
	_uint				m_iStateFlag = {};
	
protected:
	virtual void	Update_AttackColliders(_uint iStateFlag) override;

	void			Bind_HitCollisionCallback(_uint iHitCollisionIndex);
	void			Bind_AttackCollisionCallback(_uint iAttackCollisionIndex);

	void			OnCollisionAttack(const CCollider::COLLISION_DATA& CollisionData);
	void			Update_AttackCoolTime(_float fTimeDelta);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END