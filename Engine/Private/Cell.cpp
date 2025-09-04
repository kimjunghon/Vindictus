#include "EnginePch.h"
#include "Cell.h"
#include "VIBuffer_Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice { pDevice }
    , m_pDeviceContext { pDeviceContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
    m_iIndex = iIndex;

    memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_CLASS(CELL_POINT::END));

    _vector		vLine = {};

    vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]);
    m_vOutNormals[ENUM_CLASS(LINE::AB)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));
    XMStoreFloat3(&m_vInNormals[ENUM_CLASS(LINE::AB)], XMVectorScale(XMLoadFloat3(&m_vOutNormals[ENUM_CLASS(LINE::AB)]), -1.f));
    vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]);
    m_vOutNormals[ENUM_CLASS(LINE::BC)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));
    XMStoreFloat3(&m_vInNormals[ENUM_CLASS(LINE::BC)], XMVectorScale(XMLoadFloat3(&m_vOutNormals[ENUM_CLASS(LINE::BC)]), -1.f));

    vLine = XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]) - XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]);
    m_vOutNormals[ENUM_CLASS(LINE::CA)] = _float3(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine));
    XMStoreFloat3(&m_vInNormals[ENUM_CLASS(LINE::CA)], XMVectorScale(XMLoadFloat3(&m_vOutNormals[ENUM_CLASS(LINE::CA)]), -1.f));


#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pDeviceContext, pPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool CCell::IsInCell(_fvector vPosition, _int* pNeighborIndex, _float3** ppInNormal)
{
    for (_uint i = 0; i < ENUM_CLASS(LINE::END); ++i)
    {
        _vector vComputePosition = XMVectorSetY(vPosition, m_vPoints[i].y);

        _vector	vDir = XMVector3Normalize(vComputePosition - XMVectorSetW(XMLoadFloat3(&m_vPoints[i]), 1.f));
        _vector vOutNormal = XMVector3Normalize(XMLoadFloat3(&m_vOutNormals[i]));

        if (0.f < XMVectorGetX(XMVector3Dot(vDir, vOutNormal)))
        {
            *pNeighborIndex = m_iNeighborIndices[i];

            if(nullptr != ppInNormal)  
                *ppInNormal = &m_vInNormals[i];

            return false;
        }
    }

    return true;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]), vDestPoint))
            return true;
    }

    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]), vDestPoint))
            return true;
    }

    if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]), vSourPoint))
    {
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]), vDestPoint))
            return true;
        if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]), vDestPoint))
            return true;
    }

    return false;
}

_float CCell::Compute_Height(_fvector vLocalPos)
{
    _vector		vPlane = XMPlaneFromPoints(
        XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::A)]), 1.f),
        XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::B)]), 1.f),
        XMVectorSetW(XMLoadFloat3(&m_vPoints[ENUM_CLASS(CELL_POINT::C)]), 1.f));

    return (XMVectorGetX(vPlane) * -1.f * XMVectorGetX(vLocalPos) - vPlane.m128_f32[2] * vLocalPos.m128_f32[2] - vPlane.m128_f32[3]) / vPlane.m128_f32[1];
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_Resources();
    m_pVIBuffer->Render();

    return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CCell* pInstance = new CCell(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX(TEXT("Failed to Created : CCell"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);
}
