#include "EnginePch.h"
#include "BoundingSphere.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"

CBoundingSphere::CBoundingSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBounding { pDevice, pDeviceContext }
{
}

HRESULT CBoundingSphere::Initialize(const BOUNDING_DESC* pDesc)
{
	const BOUNDING_SPHERE_DESC* pSphereDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pDesc);

	m_pOriginalDesc = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pOriginalDesc);

	return S_OK;
}

void CBoundingSphere::Update(_fmatrix WorldMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBoundingSphere::Intersect(COLLIDER eType, CBounding* pTarget, _float* fDistance, _float3* pNormal)
{
	_bool isColl = false;

	switch (eType)
	{
	case COLLIDER::AABB:
		isColl = m_pDesc->Intersects(*static_cast<CBoundingAABB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::OBB:
		isColl = m_pDesc->Intersects(*static_cast<CBoundingOBB*>(pTarget)->Get_Desc());
		break;
	case COLLIDER::SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<CBoundingSphere*>(pTarget)->Get_Desc());
		break;
	}

	return isColl;
}

#ifdef _DEBUG

HRESULT CBoundingSphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

CBoundingSphere* CBoundingSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
	CBoundingSphere* pInstance = new CBoundingSphere(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed Created : CBoundingSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingSphere::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
