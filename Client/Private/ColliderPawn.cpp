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

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Container"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderContainer))))
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
	Safe_Release(m_pColliderContainer);

	for (auto& Pair : m_Colliders)
	{
		for (auto& pCollider : Pair.second)
		{
			Safe_Release(pCollider);
		}
	}
	m_Colliders.clear();
}
