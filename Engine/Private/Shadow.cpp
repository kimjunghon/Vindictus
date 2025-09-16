#include "EnginePch.h"
#include "Shadow.h"
#include "GameInstance.h"

CShadow::CShadow()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CShadow::Update_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc)
{
    m_ShadowLightDesc = ShadowLightDesc;

    return S_OK;
}

void CShadow::Update()
{
    const _float4* pWorldPoints = m_pGameInstance->Get_Frustum_WorldPoints();

    _vector vAt = {};
    _vector vEye = {};
    _vector vLightDir = {};
    
    vAt = Compute_Center(pWorldPoints);

    vLightDir = XMVector3Normalize(XMLoadFloat4(&m_ShadowLightDesc.vDirection));
    
    vEye = XMVectorSubtract(vAt, XMVectorScale(vLightDir, m_ShadowLightDesc.fDistance));
    
    if (XMVector3Equal(vEye, vAt))
        return;

    XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)], XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::PROJ)], Compute_ShadowLight_Proj(pWorldPoints));
}

_vector CShadow::Compute_Center(const _float4* pWorldPoints)          
{
    _vector vCenterPos = XMVectorZero();

    for (_uint i = 0; i < 8; i++)
    {
        vCenterPos = XMVectorAdd(vCenterPos, XMLoadFloat4(&pWorldPoints[i]));
    }

    vCenterPos = XMVectorScale(vCenterPos, (1.f / 8.f));
    vCenterPos = XMVectorSetW(vCenterPos, 1.f);

    return vCenterPos;
}

_matrix CShadow::Compute_ShadowLight_Proj(const _float4* pWorldPoints)
{
    _float4 vViewPoints[8] = {};

    for (_uint i = 0; i < 8; i++)
    {
        XMStoreFloat4(&vViewPoints[i], XMVector3TransformCoord(XMLoadFloat4(&pWorldPoints[i]), XMLoadFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)])));
    }

    cout << "Shadow" << vViewPoints[0].x << endl;
    cout << "Shadow" << vViewPoints[0].y << endl;


    _float fMinX = FLT_MAX, fMaxX = FLT_MAX * -1.f;
    _float fMinY = FLT_MAX, fMaxY = FLT_MAX * -1.f;
    _float fMinZ = FLT_MAX, fMaxZ = FLT_MAX * -1.f;

    for (_uint i = 0; i < 8; i++)
    {
        fMinX = min(fMinX, vViewPoints[i].x);
        fMaxX = max(fMaxX, vViewPoints[i].x);

        fMinY = min(fMinY, vViewPoints[i].y);
        fMaxY = max(fMaxY, vViewPoints[i].y);

        fMinZ = min(fMinZ, vViewPoints[i].z);
        fMaxZ = max(fMaxZ, vViewPoints[i].z);
    }

    _float fNear = fMinZ;
    _float fFar = fMaxZ;

    if (fNear > fFar)
        swap(fNear, fFar);

    m_fShadowLightFar = fFar;

    return XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fNear, fFar);
}

CShadow* CShadow::Create()
{
    return new CShadow();
}

void CShadow::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
