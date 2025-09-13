#pragma once
#include "Monster.h"

NS_BEGIN(Client)

using namespace Glasgavelen;

class CGlasgavelenSword;
class CBody;

class CGlasgavelen final : public CMonster
{
private:
	enum class NORMAL_ATTACK { DESEND, BLAZE, DOUBLE, GRAP, HANG, END};
	enum class RAGE_ATTACK { DESEND, COMBO, DOUBLE, BLAZE, GRAP, HANG, END};
	enum class BODY_COLLIDER {L_ARM, R_ARM, L_LEG, R_LEG, END};
	enum class HIT_COLLIDER { HEAD, L_UPPER_ARM, L_ARM, R_UPPER_ARM, R_ARM, L_LEG, R_LEG, END };
	enum class ATTACK_COLLIDER { L_SWORD, R_SWORD, END };
	enum class GRAP_COLLIDER { R_UPPER_ARM, END};
	
private:
	CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelen(const CGlasgavelen& Prototype);
	virtual ~CGlasgavelen() = default;

public:
	void					SetIsGrap(_bool IsGrap) { m_IsGrap = IsGrap; }
	_bool					IsGrap() { return m_IsGrap; }
	void					WingBreak();
	void					Change_BrokenModel();

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT	Spawn(MONSTER_SPAWN_DATA SpawnData) override;
	virtual void	Update_AttackCoolTime(_float fTimeDelta) override;
	
public:
	BT_STATE				IsLook();
	BT_STATE				Turn();

	BT_STATE				Is_Rage();
	BT_STATE				CanRageAttack();
	BT_STATE				Rage_Attack();
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;
	BT_STATE				Idle();

private:
	CGlasgavelenSword*			m_pSword = { nullptr };
	_bool						m_IsBroken = {};
	_bool						m_IsRage = {};
	_bool						m_IsGrap = {};
	_bool						m_IsSwing = {};

	GAVELEN_STATUS				m_GavelenStatus = {};
	GRAP_DATA					m_GrapData = {};

	_uint						m_iNumRageAttack = {};
	vector<_float>				m_RageAttackTime = {};
	vector<_float>				m_RageAttackCoolTime = {};

	unordered_map<COLLIDER_CHANNEL, vector<string>> m_ColliderBoneNames;


private:
	virtual HRESULT		Add_Bone_Collider(COLLIDER_CHANNEL eChannel, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, const string& strSocketName);

	HRESULT				Ready_PawnObjects();
	HRESULT				Ready_AI();
	HRESULT				Ready_GavelenStates();

	HRESULT				Ready_Collider();
	HRESULT				Ready_Collider_Bounding();
	HRESULT				Ready_Collider_Body();
	HRESULT				Ready_Collider_Hit();
	HRESULT				Ready_Collider_Attack();

	void				CreateStone(ATTACK_TYPE eType, _float fAttackRatio);
	void				CreateEneryBall(ATTACK_TYPE eType, _float fAttackRatio);
	void				ThrowStone();
	HRESULT				Add_StoneNotify(const string& strAnimName, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition);
	HRESULT				Add_GrapNotify(const string& strAnimName, _float2 vTrackPosition);
	HRESULT				Add_GrapEndNotify(const string& strAnimName, _float fAttackRatio, _float fTrackPosition);
	HRESULT				Add_EnergyBallNotify(const string& strAnimName, ATTACK_TYPE eType, _float fAttackRatio, _float fTrackPosition);
	virtual HRESULT		Add_AttackCollisionNotify(const string& strAnimName, _uint iAttackColliderIndex, ATTACK_TYPE eType, _float fAttackRatio, _float2 vTrackPosition) override;

	void				Change_ColliderSocketMatrix();
	void				Compute_WorldMatrix();
	void				OnCollisionGrap(const CCollider::COLLISION_DATA& CollisionData);
	void				OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData);
	void				DecreaseHealth(_float fDamage);

public:
	static CGlasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END