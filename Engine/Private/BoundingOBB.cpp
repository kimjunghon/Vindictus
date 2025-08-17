#include "EnginePch.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"
#include "BoundingAABB.h"

CBoundingOBB::CBoundingOBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBounding { pDevice, pDeviceContext }
{
}

HRESULT CBoundingOBB::Initialize(const BOUNDING_DESC* pDesc)
{
	const BOUNDING_OBB_DESC* pOBBDesc = static_cast<const BOUNDING_OBB_DESC*>(pDesc);

	_float4		vQuaternion = {};

	XMStoreFloat4(&vQuaternion, XMQuaternionRotationRollPitchYaw(pOBBDesc->vAngles.x, pOBBDesc->vAngles.y, pOBBDesc->vAngles.z));

	m_pOriginalDesc = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, vQuaternion);
	m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

	return S_OK;
}

void CBoundingOBB::Update(_fmatrix WorldMatrix)
{
	m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
	
}

_bool CBoundingOBB::Intersect(COLLIDER eType, CBounding* pTarget)
{
	_bool		isColl = { false };

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

HRESULT CBoundingOBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
	DX::Draw(pBatch, *m_pDesc, vColor);

	return S_OK;
}

#endif

CBoundingOBB* CBoundingOBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
	CBoundingOBB* pInstance = new CBoundingOBB(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX(TEXT("Failed Created : CBoundingOBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBoundingOBB::Free()
{
	__super::Free();

	Safe_Delete(m_pDesc);
	Safe_Delete(m_pOriginalDesc);
}
