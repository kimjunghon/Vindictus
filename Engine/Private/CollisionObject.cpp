#include "EnginePch.h"
#include "CollisionObject.h"


CCollisionObject::CCollisionObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
}

CCollisionObject::CCollisionObject(const CCollisionObject& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CCollisionObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCollisionObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_BoundingBox = Compute_BoundingVolume();

	return S_OK;
}

void CCollisionObject::Priority_Update(_float fTimeDelta)
{
}

void CCollisionObject::Update(_float fTimeDelta)
{
}

void CCollisionObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CCollisionObject::Render()
{
	return E_NOTIMPL;
}

BoundingBox CCollisionObject::Compute_BoundingVolume() const
{
	_float3 vCenter = {};
	XMStoreFloat3(&vCenter, m_pTransformCom->Get_State(STATE::POSITION));
	
	BoundingBox TempBoundingBox{};
	TempBoundingBox.Center = vCenter;

	_float3 Scaled = m_pTransformCom->Get_Scaled();
	_float fMaxScale = max(Scaled.x, max(Scaled.y, Scaled.z));
	
	TempBoundingBox.Extents = _float3(fMaxScale, fMaxScale, fMaxScale);

	return TempBoundingBox;
}


void CCollisionObject::Free()
{
}