#pragma once
#include "Client_Defines.h"
#include "Pawn.h"

NS_BEGIN(Engine)
class CNavigation;
class CCollider;
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
	HIT_TYPE		Get_HitType() { return m_eHitType; }

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
	ATTACK_MAPPING				m_AttackMapping;
	HIT_TYPE					m_eHitType = { HIT_TYPE::END };
	_vector						m_vPrevPosition = {};

protected:
	virtual void	Update_Colliders(_uint iStateFlag);
	virtual void	Update_BoundingColliders();
	virtual void	Update_BodyColliders();
	virtual void	Update_HitColliders();
	virtual void	Update_AttackColliders(_uint iStateFlag);

	void			OnCollisionBlock(const CCollider::COLLISION_DATA& ColliderData);

	HRESULT			Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);
	HRESULT			Add_Collider_Attack(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix);
	HIT_TYPE		Compute_HitType(_fvector vHitPosition, _fvector vAttackPosition);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END