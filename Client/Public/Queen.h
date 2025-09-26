#pragma once
#include "Monster.h"

using namespace Queen;

NS_BEGIN(Client)

class CQueen final : public CMonster
{
private:
	enum class QUEEN_ATTACK { SWOOP, DOUBLE, JUMP, LEFTHAND, RIGHTHAND, MELEE, END };

	//POISON, 
	enum class BODY_HIT_COLLIDER { HEAD, BODY, L_FRONT_LEG, R_FRONT_LEG, SIDE_LEG, END};
	//enum class HIT_COLLIDER { HEAD, BODY, L_FRONT_LEG, R_FRONT_LEG, SIDE_LEG, END };

	enum class ATTACK_COLLIDER { HEAD, BODY, LEFT_LEG, RIGHT_LEG, TAIL, END };

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

public:
	void			End_CutScene();
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) override;
	virtual void	Dead() override;

public:
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;
	
	BT_STATE				CanBurrow();
	BT_STATE				Burrow();

	BT_STATE				CanNearAttack();
	BT_STATE				NearAttack();

	BT_STATE				IsLook();
	BT_STATE				Turn();

	BT_STATE				Idle();
	
private:
	QUEEN_STATUS			m_QueenStatus = {};
	_uint					m_iDownFlag = {};
	_float					m_fBurrowTime = {};
	_float					m_fBurrowCoolTime = {};
	_bool					m_IsBurrow = {};
	_bool					m_IsEyeLight = {};

	_matrix					m_AnimRotationMatrix = {};

	_float					m_fNearAttackTime = {};
	_float					m_fNearAttackCoolTime = {};

	vector<const _float4x4*>	m_BodyColliderSocketMatrix;
	vector<_matrix>				m_BodyColliderCombinedMatrix;

private:
	virtual void	Update_AttackCoolTime(_float fTimeDelta) override;
	void			MoveTarget(_float fRatio);

	HRESULT			Ready_PawnObjects();
	HRESULT			Ready_AI();
	HRESULT			Ready_QueenStates();

	HRESULT			Ready_Collider();
	HRESULT			Ready_Collider_Bounding();
	HRESULT			Ready_Collider_Body_Hit();
	HRESULT			Ready_Collider_Attack();

	void			Compute_WorldMatrix();

	void			OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData);
	void			DecreaseDurabillity(_uint HitColliderIndex, _float fDamage);

	void			Check_Near(_float fTimeDelta);
	HRESULT			Change_Camera();
	HRESULT			Request_SpawnEyeTrail();
	
	void			Dying();


public:
	static CQueen*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END