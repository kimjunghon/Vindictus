#include "ClientPch.h"
#include "ColliderPawn.h"

CColliderPawn::CColliderPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawn { pDevice, pDeviceContext }
{
}

CColliderPawn::CColliderPawn(const CColliderPawn& Prototype)
	: CPawn { Prototype }
	, m_AttackMapping{ Prototype.m_AttackMapping }
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


void CColliderPawn::Update_Colliders(_uint iStateFlag)
{
	Update_BoundingColliders();
	Update_BodyColliders();
	Update_HitColliders();
	Update_AttackColliders(iStateFlag);
}

void CColliderPawn::Update_BoundingColliders()
{
	for (auto& pBoundingCollider : m_Colliders[COLLIDER_CHANNEL::BOUNDING])
	{
		pBoundingCollider->SetEnable(true);
		pBoundingCollider->Update(m_pTransformCom->Get_WorldMatrix());
		m_pGameInstance->Add_BoundingCollider(this, pBoundingCollider);
	}
}

void CColliderPawn::Update_BodyColliders()
{
	for (auto& pBodyCollider : m_Colliders[COLLIDER_CHANNEL::BODY])
	{
		pBodyCollider->SetEnable(true);
		pBodyCollider->Update(m_pTransformCom->Get_WorldMatrix());
		m_pGameInstance->Add_ActionCollider(this, pBodyCollider);
	}
}

void CColliderPawn::Update_HitColliders()
{
	for (_uint i = 0; i < m_Colliders[COLLIDER_CHANNEL::HIT].size(); i++)
	{
		m_HitColliderCombinedMatrix[i] = XMMatrixMultiply(XMLoadFloat4x4(m_HitColliderSocketMatrix[i]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::HIT][i]->Update(m_HitColliderCombinedMatrix[i]);

		m_Colliders[COLLIDER_CHANNEL::HIT][i]->SetEnable(true);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::HIT][i]);
	}
}

void CColliderPawn::Update_AttackColliders(_uint iStateFlag)
{
	auto iter = m_AttackMapping.find(iStateFlag);
	if (iter == m_AttackMapping.end())
		return;

	for (auto& AttackMap : iter->second)
	{
		CCollider::ATTACK_COLLISON_DATA AttackData = {};
		AttackData.IsDown = AttackMap.IsDown;
//		AttackData.fDamage = 10.f;//m_fAttackData * AttackMap.fAttackRatio;
		AttackData.vAttackPosition = m_pTransformCom->Get_State(STATE::POSITION);

		_uint iAttackIndex = AttackMap.iAttackColliderIndex;

		m_AttackColliderCombinedMatrix[iAttackIndex] = XMMatrixMultiply(XMLoadFloat4x4(m_AttackColliderSocketMatrix[iAttackIndex]), m_pTransformCom->Get_WorldMatrix());
		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->Update(m_AttackColliderCombinedMatrix[iAttackIndex]);

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetEnable(true);
		m_pGameInstance->Add_ActionCollider(this, m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]);

		m_Colliders[COLLIDER_CHANNEL::ATTACK][iAttackIndex]->SetAttackData(AttackData);
	}
}

void CColliderPawn::OnCollisionBlock(const CCollider::COLLISION_DATA& ColliderData)
{
	_vector vNormal = XMVector3Normalize(XMVectorSetY(XMLoadFloat3(&ColliderData.BlockData.vNormal), 0.f));
	_vector vCollision = XMVectorScale(vNormal, ColliderData.BlockData.fDistance);

	_vector vPosition = XMVectorSubtract(m_pTransformCom->Get_State(STATE::POSITION), vCollision);

	m_pTransformCom->Set_State(STATE::POSITION, vPosition);

	m_pTransformCom->Sliding(vCollision, vNormal, m_pNavigationCom);
}

HRESULT CColliderPawn::Add_Collider_Hit(const _wstring& strColliderTag, COLLIDER_OWNER eOwner, CBoundingOBB::BOUNDING_OBB_DESC* pDesc, _uint iColliderIndex, const _float4x4* pSocketCombinedMatrix)
{
	if (iColliderIndex >= m_Colliders[COLLIDER_CHANNEL::HIT].size() ||
		nullptr == pSocketCombinedMatrix)
		return E_FAIL;

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.eChannel = COLLIDER_CHANNEL::HIT;
	ColliderDesc.eOwner = eOwner;
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
	ColliderDesc.eChannel = COLLIDER_CHANNEL::ATTACK;
	ColliderDesc.eOwner = eOwner;
	ColliderDesc.BoundingDesc = pDesc;

	CCollider* pAttackCollider = { nullptr };

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_OBB"),
		strColliderTag, reinterpret_cast<CComponent**>(&pAttackCollider), &ColliderDesc)))
		return E_FAIL;

	m_Colliders[COLLIDER_CHANNEL::ATTACK][iColliderIndex] = pAttackCollider;
	m_AttackColliderSocketMatrix[iColliderIndex] = pSocketCombinedMatrix;

	return S_OK;
}

HIT_TYPE CColliderPawn::Compute_HitType(_fvector vHitPosition, _fvector vAttackPosition)
{
	_vector vHitDir = XMVector3Normalize(XMVectorSetY(XMVectorSubtract(vAttackPosition, vHitPosition), 0.f));
	_vector vLook = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(STATE::LOOK), 0.f));
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	_float	fLookDot = XMVectorGetX(XMVector3Dot(vLook, vHitDir));
	_float	fRightDot = XMVectorGetX(XMVector3Dot(vRight, vHitDir));

	_float fComparisonRadian = cosf(XMConvertToRadians(70.f));

	HIT_TYPE eHit_Type = {};

	if (fLookDot >= fComparisonRadian)
		eHit_Type = HIT_TYPE::FRONT;
	else if (fLookDot <= -fComparisonRadian)
		eHit_Type = HIT_TYPE::BACK;
	else if (fRightDot >= 0.f)
		eHit_Type = HIT_TYPE::RIGHT;
	else
		eHit_Type = HIT_TYPE::LEFT;

	return eHit_Type;
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
