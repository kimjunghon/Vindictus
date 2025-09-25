#pragma once
#include "ColliderPawn.h"

NS_BEGIN(Engine)
class CBehaviorTree;
class CNavigation;
class CCollider;
NS_END

NS_BEGIN(Client)
class CPool_Instance;
class CMonsterState;
class CBody;

class CMonster abstract : public CColliderPawn
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	_uint			Get_AttackIndex() { return m_iCurrentAttack; }
	_float			Get_MinDistance() { return m_fMinDistance; }
	_float			Get_AttackRange() { return m_fAttackRange; }
	void			Dead();
	_bool			IsAnimationInRangeTrackPosition(_float2 vRange);
	_bool			IsAnimationPassToTrackPosition(_float fTrackPosition);
	_bool			IsReadyAttack() { return m_IsReadyAttack; }
	_float			Get_TargetDistance();
	DIR 			Compute_TargetDir(_float fDegree);

	void			Set_ReadyAttack(_bool IsReady) { m_IsReadyAttack = IsReady; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) PURE;
	_bool			IsNear(_float fNearDistance);
	void			LookAtTarget();
	void			TurnToTarget(_float fRatio);
	void			MoveToTarget(_float fRatio);
	//State
public:
	_bool				AnimIsFinished();
	_bool				AnimCanChange();

	_bool				CanChangeState();
	HRESULT				ChangeState(_uint iStateIndex);

	virtual BT_STATE	CanAttack();
	virtual BT_STATE	CanOtherAction();
	virtual BT_STATE	CanAttackRange();
	virtual BT_STATE	Attack() PURE;
	virtual BT_STATE	Chase() PURE;
	virtual BT_STATE	Patrol() PURE;

protected:
	STATUS			m_Status = {};
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
	CPool_Instance*			m_pPool_Instance = { nullptr };
	MONSTER_TYPE				m_eType = {};
	CBehaviorTree*				m_pAI = { nullptr };
	CBody*						m_pBody = { nullptr };
	const _vector*				m_pAnimMovement = {nullptr};
	const _vector*				m_pAnimRotation = { nullptr };

	vector<CMonsterState*>		m_States;
	CMonsterState*				m_pCurrentState = { nullptr };
	_bool						m_IsReadyAttack = {};

	_uint						m_iStateFlag = {};
	size_t						m_iHitAttackID = {};

protected:
	void			Bind_StateFlag();

	virtual HRESULT	Ready_AnimNotify(const string& strFilePath);
	virtual HRESULT Ready_Status(const string& strFilePath);
	virtual HRESULT	Ready_EffectNotify(const string& strFilePath);
	virtual HRESULT	Ready_SoundNotify(const _char* pFilePath);

	virtual HRESULT	Add_ReadyAttackNotify(const string& strAnimName, _float2 vTrackPosition);
	virtual HRESULT	Add_AttackCollisionNotify(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, const _wstring& strHitSoundName, _float fAttackRatio, _float2 vTrackPosition);
	virtual HRESULT Add_SoundNotify(const string& strAnimName, _float fTrackPosition, _float fVolume, const _wstring& strSoundName, _bool IsVoice, const _wstring& strVoiceName);


	void			OnCollisionAttack(const CCollider::COLLISION_DATA& CollisionData);
	virtual void	Update_AttackCoolTime(_float fTimeDelta);
	void			SpawnHitEffect(const CCollider::COLLISION_DATA& CollisionData, const ATTACK_EFFECT_DATA& EffectData);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END