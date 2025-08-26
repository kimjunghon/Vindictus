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
	enum class ATTACK_COLLIDER { L_UPPER_ARM, L_SWORD, R_UPPER_ARM, R_SWORD, END };


private:
	CGlasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelen(const CGlasgavelen& Prototype);
	virtual ~CGlasgavelen() = default;

public:
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

	
public:
	BT_STATE				Is_Rage();
	BT_STATE				Rage_Attack();
	virtual BT_STATE		Attack() override;
	virtual BT_STATE		Chase() override;
	virtual BT_STATE		Patrol() override;
	BT_STATE				Idle();

private:
	CGlasgavelenSword*			m_pSword = { nullptr };
	_bool						m_IsRage = {};
	_bool						m_IsGrap = {};

	GAVELEN_STATUS				m_GavelenStatus = {};

	_uint						m_iNumRageAttack = {};
	vector<_float>				m_RageAttackTime = {};
	vector<_float>				m_RageAttackCoolTime = {};

	vector<const _float4x4*>	m_BodyColliderSocketMatrix;
	vector<_matrix>				m_BodyColliderCombinedMatrix;
	
	vector<string>				m_BodyColliderSocketName;
	vector<string>				m_HitColliderSocketName;
	vector<string>				m_AttackColliderSocketName;

private:
	virtual void	Update_BodyColliders(_fmatrix UpdateWorldMatrix) override;
	virtual HRESULT	Add_Collider_Body(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName);
	virtual HRESULT	Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName);
	virtual HRESULT	Add_Collider_Attack(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const string& strSocketName);

	HRESULT Ready_PawnObjects();
	HRESULT	Ready_AI();
	HRESULT	Ready_GavelenStates();

	HRESULT	Ready_Collider();
	HRESULT	Ready_Collider_Bounding();
	HRESULT	Ready_Collider_Body();
	HRESULT	Ready_Collider_Hit();
	HRESULT	Ready_Collider_Attack();
	HRESULT	Ready_AttackMapping();

	void	Change_ColliderSocketMatrix();
	void	Compute_WorldMatrix();
	void	OnCollisionHit(_uint HitColliderIndex, const CCollider::COLLISION_DATA& CollisionData);

public:
	static CGlasgavelen*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END