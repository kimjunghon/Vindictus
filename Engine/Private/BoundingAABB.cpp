#include "EnginePch.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "BoundingSphere.h"

CBoundingAABB::CBoundingAABB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CBounding { pDevice, pDeviceContext }
{
}

HRESULT CBoundingAABB::Initialize(const BOUNDING_DESC* pDesc)
{
    const BOUNDING_AABB_DESC* pAABBDesc = static_cast<const BOUNDING_AABB_DESC*>(pDesc);

    m_pOriginalDesc = new BoundingBox(pAABBDesc->vCenter, pAABBDesc->vExtents);
    m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

void CBoundingAABB::Update(_fmatrix WorldMatrix, _bool IsRotate)
{
    _matrix TransformMatrix = WorldMatrix;

    TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(TransformMatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(TransformMatrix.r[2]);

    m_pOriginalDesc->Transform(*m_pDesc, TransformMatrix);
}

_bool CBoundingAABB::Intersect(COLLIDER eType, CBounding* pTarget, _float* pDistance, _float3* pNormal)
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

HRESULT CBoundingAABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _fvector vColor)
{
    DX::Draw(pBatch, *m_pDesc, vColor);

    return S_OK;
}

#endif

CBoundingAABB* CBoundingAABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
    CBoundingAABB* pInstance = new CBoundingAABB(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX(TEXT("Failed Created : CBoundingAABB"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBoundingAABB::Free()
{
    __super::Free();

    Safe_Delete(m_pDesc);
    Safe_Delete(m_pOriginalDesc);
}
