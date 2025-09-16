#include "EnginePch.h"
#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
    m_vPoints[0] = _float4(-1.f, 1.f, 0.f, 1.f);
    m_vPoints[1] = _float4(1.f, 1.f, 0.f, 1.f);
    m_vPoints[2] = _float4(1.f, -1.f, 0.f, 1.f);
    m_vPoints[3] = _float4(-1.f, -1.f, 0.f, 1.f);

    m_vPoints[4] = _float4(-1.f, 1.f, 1.f, 1.f);
    m_vPoints[5] = _float4(1.f, 1.f, 1.f, 1.f);
    m_vPoints[6] = _float4(1.f, -1.f, 1.f, 1.f);
    m_vPoints[7] = _float4(-1.f, -1.f, 1.f, 1.f);

    return S_OK;
}

void CFrustum::Update()
{
    _matrix ViewMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::VIEW);
    _matrix ProjMatrixInv = m_pGameInstance->Get_Transform_Matrix_Inverse(D3DTS::PROJ);

    for (_uint i = 0; i < 8; i++)
    {
        XMStoreFloat4(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_vPoints[i]), ProjMatrixInv));
        XMStoreFloat4(&m_vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&m_vWorldPoints[i]), ViewMatrixInv));
    }
    Update_WorldPlanes();
}

void CFrustum::Update_WorldPlanes()
{
    XMStoreFloat4(&m_vWorldPlanes[0],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[1]), XMLoadFloat4(&m_vWorldPoints[5]), XMLoadFloat4(&m_vWorldPoints[6])));
    XMStoreFloat4(&m_vWorldPlanes[1],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[4]), XMLoadFloat4(&m_vWorldPoints[0]), XMLoadFloat4(&m_vWorldPoints[3])));
    XMStoreFloat4(&m_vWorldPlanes[2],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[4]), XMLoadFloat4(&m_vWorldPoints[5]), XMLoadFloat4(&m_vWorldPoints[1])));
    XMStoreFloat4(&m_vWorldPlanes[3],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[3]), XMLoadFloat4(&m_vWorldPoints[2]), XMLoadFloat4(&m_vWorldPoints[6])));
    XMStoreFloat4(&m_vWorldPlanes[4],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[5]), XMLoadFloat4(&m_vWorldPoints[4]), XMLoadFloat4(&m_vWorldPoints[7])));
    XMStoreFloat4(&m_vWorldPlanes[5],
        XMPlaneFromPoints(XMLoadFloat4(&m_vWorldPoints[0]), XMLoadFloat4(&m_vWorldPoints[1]), XMLoadFloat4(&m_vWorldPoints[2])));
}

CFrustum* CFrustum::Create()
{
    CFrustum* pInstance = new CFrustum();
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed Created : CFrustum"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFrustum::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);
}
