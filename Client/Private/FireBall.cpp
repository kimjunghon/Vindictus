#include "ClientPch.h"
#include "FireBall.h"

CFireBall::CFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProjectile { pDevice, pDeviceContext }
{
}

CFireBall::CFireBall(const CFireBall& Prototype)
	: CProjectile { Prototype }
	, m_fEffectTime { Prototype.m_fEffectTime }
{
}

HRESULT CFireBall::Initialize_Prototype()
{
	m_fEffectTime = 0.05f;

	return S_OK;
}

HRESULT CFireBall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CFireBall::Priority_Update(_float fTimeDelta)
{
}

void CFireBall::Update(_float fTimeDelta)
{
	if (*m_pIsSwing)
	{
		m_IsMove = true;
		Update_Bezier();
		*m_pIsSwing = false;
	}

	if (m_IsMove)
		CProjectile::Move(fTimeDelta);
	else
		m_pTransformCom->Set_WorldMatrix(XMMatrixMultiply(XMLoadFloat4x4(m_pSocektMatrixPtr), XMLoadFloat4x4(m_pOwnerMatrixPtr)));

	_matrix EffectWorldMatrix = m_pTransformCom->Get_WorldMatrix();

	m_fCurrentEffectTime += fTimeDelta;

	if(m_fCurrentEffectTime >= m_fEffectTime)
	{
		if (FAILED(m_pPoolInstance->Request_SpawnEffect(TEXT("FireBall"), &EffectWorldMatrix)))
			return;
		m_fCurrentEffectTime = 0.f;
	}

	//test
	if (XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) <= 0.f)
		ReturnToPool();
}

void CFireBall::Late_Update(_float fTimeDelta)
{
//	m_pGameInstance->Add_ActionCollider()
}

HRESULT CFireBall::Render()
{
	return S_OK;
}

HRESULT CFireBall::Spawn(void* pArg)
{
	FIREBALL_DESC* pDesc = static_cast<FIREBALL_DESC*>(pArg);

	m_IsActive = true;

	m_eType = pDesc->eType;
	m_fDamage = pDesc->fDamage;
	m_pTargetTransformCom = pDesc->pTargetTransform;

	m_pIsSwing = pDesc->pIsSwing;
	m_pSocektMatrixPtr = pDesc->pSocketMatrixPtr;
	m_pOwnerMatrixPtr = pDesc->pOwnerMatrixPtr;

	m_fCurrentDistance = 0.f;
	m_IsCurveFinished = false;

	m_fCurrentEffectTime = 0.f;

	m_LUT.clear();

	return S_OK;
}

void CFireBall::ReturnToPool()
{
	m_pPoolInstance->ReturnPool(TEXT("FireBall"), this);
	m_IsMove = false;
	m_IsActive = false;
	m_pIsSwing = nullptr;
	m_pSocektMatrixPtr = nullptr;
	m_pOwnerMatrixPtr = nullptr;
	m_pTargetTransformCom = nullptr;
}

HRESULT CFireBall::Ready_Component()
{
	CBoundingSphere::BOUNDING_SPHERE_DESC BoundingDesc = {};
	BoundingDesc.fRadius = 10.f;
	BoundingDesc.vCenter = _float3(0.f, 0.f, 0.f);

	CCollider::COLLIDER_DESC ColliderDesc = {};
	ColliderDesc.iChannel = ENUM_CLASS(COLLIDER_CHANNEL::ATTACK);
	ColliderDesc.iOwner = ENUM_CLASS(COLLIDER_OWNER::MONSTER);
	ColliderDesc.BoundingDesc = &BoundingDesc;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CFireBall::Update_Bezier()
{
	_vector vStartPosition = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vFinishPosition = m_pTargetTransformCom->Get_State(STATE::POSITION);

	_vector vDir = XMVector3Normalize((XMVectorSubtract(vFinishPosition, vStartPosition)));
	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vDir);
	_vector vUp = XMVector3Normalize(XMVector3Cross(vDir, vRight));

	vFinishPosition = vFinishPosition + (vDir * 10.f);

	_float fLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(vFinishPosition, vStartPosition)));

	_float fLookWieght = fLength * 0.2f;
	_float fUpWieght = fLength * 0.3f;

	vFinishPosition = vFinishPosition + XMVectorSet(0.f, 20.f, 0.f, 0.f);

	_vector vPoints[4] = {};

	vPoints[0] = vStartPosition;
	vPoints[1] = vStartPosition + (vDir * fLookWieght) + (vUp * fUpWieght);
	vPoints[2] = vPoints[1] + (vDir * fLookWieght);//vFinishPosition - (vDir * fLookWieght +(vUp * fUpWieght);
	vPoints[3] = vFinishPosition;

	CProjectile::Ready_Bezier(4, vPoints);
}

CFireBall* CFireBall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFireBall* pInstance = new CFireBall(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CFireBall"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFireBall::Clone(void* pArg)
{
	CFireBall* pInstance = new CFireBall(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CFireBall"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireBall::Free()
{
	__super::Free();
}
