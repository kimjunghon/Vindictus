#pragma once
#include "Client_Defines.h"
#include "Pawn.h"
#include "Collider.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CColliderPawn abstract : public CPawn
{
protected:
	typedef unordered_map<COLLIDER_CHANNEL, vector<CCollider*>> COLLIDER;

protected:
	CColliderPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColliderPawn(const CColliderPawn& Prototype);
	virtual ~CColliderPawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CNavigation*				m_pNavigationCom = { nullptr };

	COLLIDER					m_Colliders;
	vector<const _float4x4*>	m_HitColliderSocketMatrix;
	vector<_matrix>				m_HitColliderCombinedMatrix;
	vector<const _float4x4*>	m_AttackColliderSocketMatrix;
	vector<_matrix>				m_AttackColliderCombinedMatrix;

	ATTACK_DATA					m_CurrentAttackData = {};

	_vector						m_vPrevPosition = {};

protected:
	virtual void	Update_Colliders(_fmatrix UpdateWorldMatrix);
	virtual void	Update_BoundingColliders(_fmatrix UpdateWorldMatrix);
	virtual void	Update_BodyColliders(_fmatrix UpdateWorldMatrix);
	virtual void	Update_HitColliders(_fmatrix UpdateWorldMatrix);
	virtual void	Update_AttackColliders(_fmatrix UpdateWorldMatrix);
	virtual void	Update_GrapColliders(_fmatrix UpdateWorldMatrix);

	void			EnableAllColliderChannel();
	void			EnableColliderChannel(COLLIDER_CHANNEL eChannel);

	void			DisableAllColliderChannel();
	void			DisableColliderChannel(COLLIDER_CHANNEL eChannel);

	void			OnCollisionBlock(const CCollider::COLLISION_DATA& ColliderData);

	virtual HRESULT	Add_Collider_Bounding(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingAABB::BOUNDING_AABB_DESC* pDesc);
	virtual HRESULT	Add_Collider_Body(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc);
	virtual HRESULT	Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);
	virtual HRESULT	Add_Collider_Attack(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);

	HRESULT			Bind_Collision_Callback(COLLIDER_CHANNEL eChannel, _uint iColliderIndex, COLLIDER_STATE eState, CCollider::Collision_CallBack Callback);

	DIR				Compute_HitDir(_fvector vHitPosition, _fvector vAttackPosition, _float fDegree);
	DIR				Compute_HitDir_Look(_fvector vHitPosition, _fvector vAttackPosition);
	DIR				Compute_HitDir_Side(_fvector vHitPosition, _fvector vAttackPosition);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END