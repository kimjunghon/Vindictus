#include "ClientPch.h"
#include "ColliderPawn.h"
#include "Body.h"

CColliderPawn::CColliderPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawn { pDevice, pDeviceContext }
{
}

CColliderPawn::CColliderPawn(const CColliderPawn& Prototype)
	: CPawn { Prototype }
{
}

HRESULT CColliderPawn::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CColliderPawn::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CColliderPawn::Priority_Update(_float fTimeDelta)
{
}

void CColliderPawn::Update(_float fTimeDelta)
{
}

void CColliderPawn::Late_Update(_float fTimeDelta)
{
}

HRESULT CColliderPawn::Render()
{
	return S_OK;
}


void CColliderPawn::Update_Colliders(_fmatrix UpdateWorldMatrix)
{
	Update_BoundingColliders(UpdateWorldMatrix);
	Update_BodyColliders(UpdateWorldMatrix);
	Update_HitColliders(UpdateWorldMatrix);
	Update_AttackColliders(UpdateWorldMatrix);
	Update_GrapColliders(UpdateWorldMatrix);
}

void CColliderPawn::Update_BoundingColliders(_fmatrix UpdateWorldMatrix)
{
	for (auto& pBoundingCollider : m_Colliders[COLLIDER_CHANNEL::BOUNDING])
	{
		if (false == pBoundingCollider->IsEnable())
			continue;

		pBoundingCollider->Update(UpdateWorldMatrix);
		m_pGameInstance->Add_BoundingCollider(this, pBoundingCollider);
	}
}

void CColliderPawn::Update_BodyColliders(_fmatrix UpdateWorldMatrix)
{
	for (auto& pBodyCollider : m_Colliders[COLLIDER_CHANNEL::BODY])
	{
//		if (false == pBodyCollider->IsEnable())
//			continue;

		pBodyCollider->Update(UpdateWorldMatrix);
		m_pGameInstance->Add_ActionCollider(this, pBodyCollider);
	}
}

void CColliderPawn::Update_HitColliders(_fmatrix UpdateWorldMatrix)
{
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::HIT].size(); i++)
	{
		//if (false == m_Colliders[COLLIDER_CHANNEL::HIT][i]->IsEnable())
		//	continue;

		m_HitColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_HitColliderSocketMatrix[i]), UpdateWorldMatrix);
		m_Colliders[COLLIDER_CHANNEL::HIT][i]->Update(m_HitColliderCombinedMatrix[i]);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::HIT][i]);
	}
}

void CColliderPawn::Update_AttackColliders(_fmatrix UpdateWorldMatrix)
{
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::ATTACK].size(); i++)
	{
		//if (false == m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->IsEnable())
		//	continue;

		m_AttackColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[i]), UpdateWorldMatrix);
		m_Colliders[COLLIDER_CHANNEL::ATTACK][i]->Update(m_AttackColliderCombinedMatrix[i]);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::ATTACK][i]);
	}
}

void CColliderPawn::Update_GrapColliders(_fmatrix UpdateWorldMatrix)
{
	for (auto& pGrapCollider : m_Colliders[COLLIDER_CHANNEL::GRAP])
	{
		if (false == pGrapCollider->IsEnable())
			continue;

		pGrapCollider->Update(UpdateWorldMatrix);
		m_pGameInstance->Add_ActionCollider(this, pGrapCollider);
	}
}

void CColliderPawn::EnableAllColliderChannel()
{
	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
			pCollider->SetEnable(true);
	}
}

void CColliderPawn::EnableColliderChannel(COLLIDER_CHANNEL eChannel)
{
	for (auto& pCollider : m_Colliders[eChannel])
		pCollider->SetEnable(true);
}

void CColliderPawn::DisableAllColliderChannel()
{
	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
			pCollider->SetEnable(false);
	}
}

void CColliderPawn::DisableColliderChannel(COLLIDER_CHANNEL eChannel)
{
	for (auto& pCollider : m_Colliders[eChannel])
		pCollider->SetEnable(false);
}

void CColliderPawn::OnCollisionBlock(const CCollider::COLLISION_DATA& ColliderData)
{
	_vector vPosition = m_pTransformCom->Get_State(STATE::POSITION);

	m_pTransformCom->Set_State(STATE::POSITION, m_vPrevPosition);

	_vector vNormal = XMVector3Normalize(XMLoadFloat3(&ColliderData.BlockData.vNormal));

	_vector vDir = XMVectorSubtract(vPosition, m_vPrevPosition);

	if (XMVectorGetX(XMVector3Dot(vDir, vNormal)) > 0.f)
		m_pTransformCom->MovePositionToVector(vDir, m_pNavigationCom);
	else
		m_pTransformCom->Sliding(vDir, vNormal, m_pNavigationCom);

}

HRESULT CColliderPawn::Add_Collider_Bounding(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingAABB::BOUNDING_AABB_DESC* pDesc)
{
	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::BOUNDING);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pBoundingCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_AABB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pBoundingCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::BOUNDING].push_back(pBoundingCollider);

	return S_OK;
}

HRESULT CColliderPawn::Add_Collider_Body(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc)
{
	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::BODY);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pBodyCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pBodyCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::BODY].push_back(pBodyCollider);

	return S_OK;
}

HRESULT CColliderPawn::Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::HIT].size() ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::HIT);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pHitCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pHitCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::HIT][iColliderIndex] = pHitCollider;
	m_HitColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HRESULT CColliderPawn::Add_Collider_Attack(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::ATTACK].size() ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::ATTACK);
	ColliderDesc.iOwner = ENUM_CLASS(eOwner);
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pAttackCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::ATTACK][iColliderIndex] = pAttackCollider;
	m_AttackColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HRESULT CColliderPawn::Bind_Collision_Callback(COLLIDER_CHANNEL eChannel, _uint iColliderIndex, COLLIDER_STATE eState, CCollider::Collision_CallBack Callback)
{
	if (nullptr == m_Colliders[eChannel][iColliderIndex])
		return E_FAIL;

	m_Colliders[eChannel][iColliderIndex]->SetCollisionCallBack(ENUM_CLASS(eState), Callback);

	return S_OK;
}

DIR CColliderPawn::Compute_HitDir(_fvector vHitPosition, _fvector vAttackPosition, _float fDegree)
{
	_vector vHitDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(vAttackPosition, vHitPosition), 0.f));
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	_float	fLookDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));
	_float	fRightDot = XMVectorGetX(XMVector3Dot(vRight, vHitDir));

	_float fComparisonRadian = cosf(XMConvertToRadians(fDegree));

	DIR eHit_Dir = {};

	if (fLookDot >= fComparisonRadian)
		eHit_Dir = DIR::FRONT;
	else if (fLookDot <= (fComparisonRadian * -1.f))
		eHit_Dir = DIR::BACK;
	else if (fRightDot >= 0.f)
		eHit_Dir = DIR::RIGHT;
	else
		eHit_Dir = DIR::LEFT;

	return eHit_Dir;
}

DIR CColliderPawn::Compute_HitDir_Look(_fvector vHitPosition, _fvector vAttackPosition)
{
	_vector vHitDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(vAttackPosition, vHitPosition), 0.f));
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));

	_float	fLookDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));

	DIR eHit_Dir = {};

	if (fLookDot >= 0.f)
		eHit_Dir = DIR::FRONT;
	else if (fLookDot < 0.f)
		eHit_Dir = DIR::BACK;

	return eHit_Dir;
}

DIR CColliderPawn::Compute_HitDir_Side(_fvector vHitPosition, _fvector vAttackPosition)
{
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(vAttackPosition, vHitPosition), 0.f));
	_vector vLook = XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	_float fRightDot = XMVectorGetX(XMVector3Dot(vRight, vTargetDir));

	DIR eHit_Dir = {};

	if (fRightDot >= 0.f)
		eHit_Dir = DIR::RIGHT;
	else if (fRightDot < 0.f)
		eHit_Dir = DIR::LEFT;

	return eHit_Dir;
}

void CColliderPawn::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			Safe_Release(pCollider);
		}
	}
	m_Colliders.clear();
}
