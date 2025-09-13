#pragma once
#include "Client_Defines.h"
#include "Pawn.h"
#include "Collider.h"

NS_BEGIN(Engine)
class CNavigation;
class CColliderContainer;
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
	CColliderContainer*			m_pColliderContainer = { nullptr };

	ATTACK_DATA					m_CurrentAttackData = {};

	_vector						m_vPrevPosition = {};

protected:
	void			OnCollisionBlock(const CCollider::COLLISION_DATA& ColliderData);

	DIR				Compute_HitDir(_fvector vHitPosition, _fvector vAttackPosition, _float fDegree);
	DIR				Compute_HitDir_Look(_fvector vHitPosition, _fvector vAttackPosition);
	DIR				Compute_HitDir_Side(_fvector vHitPosition, _fvector vAttackPosition);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END