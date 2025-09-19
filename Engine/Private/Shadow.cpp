#include "EnginePch.h"
#include "Shadow.h"
#include "GameInstance.h"

CShadow::CShadow()
    : m_pGameInstance { CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

//HRESULT CShadow::Bind_Shadow_ProjMatrices(CShader* pShader, const _char* pConstantName)
//{
//    if (nullptr == pShader)
//        return E_FAIL;
//
//    return pShader->Bind_Matrices(pConstantName, m_ShadowProjMatrices, 4);
//}
//
//HRESULT CShadow::Bind_SplitDistance(CShader* pShader, const _char* pConstantName)
//{
//    return pShader->Bind_RawValue(pConstantName, &m_fSplitDistance[0], sizeof(_float) * 5);
//}
//
//HRESULT CShadow::Bind_Shadow_ProjMatrix(CShader* pShader, const _char* pMatrixConstantName, const _char* pIndexConstantName, _fvector vWorldPosition)
//{
//    if (nullptr == pShader)
//        return E_FAIL;
//
//    _float fDistance = XMVectorGetZ(XMVector3TransformCoord(vWorldPosition, m_CameraViewMatrix));
//    
//    m_iSplitIndex = 0;
//    while (fDistance > m_fSplitDistance[m_iSplitIndex+1])
//        m_iSplitIndex++;
//   
//    if (FAILED(pShader->Bind_Matrix(pMatrixConstantName, &m_ShadowProjMatrices[m_iSplitIndex])))
//        return E_FAIL;
//
//    if (FAILED(pShader->Bind_RawValue(pIndexConstantName, &m_iSplitIndex, sizeof(_uint))))
//        return E_FAIL;
//
//    return S_OK;
//}

HRESULT CShadow::Initialize(_float fViewportWidth, _float fViewportHeight)
{
    m_fWidth = fViewportWidth * 2.f;
    m_fHeight = fViewportHeight * 2.f;

    return S_OK;
}

HRESULT CShadow::Update_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc)
{
    m_ShadowLightDesc = ShadowLightDesc;

    m_vDirection = XMVector3Normalize(XMLoadFloat4(&m_ShadowLightDesc.vDirection));
    m_fDistance = m_ShadowLightDesc.fDistance;

    return S_OK;
}

void CShadow::Update(_fvector vTargetPosition)
{
    _vector vAt = vTargetPosition;

    _vector vLightPos = XMVectorAdd(XMVectorScale(m_vDirection, m_fDistance), vAt);

    _vector vEye = XMVectorSubtract(vAt, vLightPos);

    XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)], XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::PROJ)], XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_ShadowLightDesc.fNear, m_ShadowLightDesc.fFar));
}
//
//void CShadow::Update()
//{
//    const _float4* pWorldPoints = m_pGameInstance->Get_Frustum_WorldPoints();
//    m_CameraViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
//
//    _vector vAt = {};
//    _vector vEye = {};
//    _vector vLightDir = {};
//    
//    vAt = Compute_Center(pWorldPoints);
//
//    vLightDir = XMVector3Normalize(XMLoadFloat4(&m_ShadowLightDesc.vDirection));
//    
//    vEye = XMVectorSubtract(vAt, XMVectorScale(vLightDir, m_ShadowLightDesc.fDistance));
//    
//    if (XMVector3Equal(vEye, vAt))
//        return;
//
//    XMStoreFloat4x4(&m_ShadowViewMatrix, XMMatrixLookAtLH(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
//
//    Make_ShadowLight_SplitProj(pWorldPoints);
//
//    //XMStoreFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::PROJ)], Compute_ShadowLight_Proj(pWorldPoints));
//}

//_vector CShadow::Compute_Center(const _float4* pWorldPoints)          
//{
//    _vector vCenterPos = XMVectorZero();
//
//    for (_uint i = 0; i < 8; i++)
//    {
//        vCenterPos = XMVectorAdd(vCenterPos, XMLoadFloat4(&pWorldPoints[i]));
//    }
//
//    vCenterPos = XMVectorScale(vCenterPos, (1.f / 8.f));
//    vCenterPos = XMVectorSetW(vCenterPos, 1.f);
//
//    return vCenterPos;
//}
//
////_matrix CShadow::Compute_ShadowLight_Proj(const _float4* pWorldPoints)
////{
////    _float4 vViewPoints[8] = {};
////
////    for (_uint i = 0; i < 8; i++)
////    {
////        XMStoreFloat4(&vViewPoints[i], XMVector3TransformCoord(XMLoadFloat4(&pWorldPoints[i]), XMLoadFloat4x4(&m_Matrices[ENUM_CLASS(D3DTS::VIEW)])));
////    }
////
////    _float fMinX = FLT_MAX, fMaxX = FLT_MAX * -1.f;
////    _float fMinY = FLT_MAX, fMaxY = FLT_MAX * -1.f;
////    _float fMinZ = FLT_MAX, fMaxZ = FLT_MAX * -1.f;
////
////    for (_uint i = 0; i < 8; i++)
////    {
////        fMinX = min(fMinX, vViewPoints[i].x);
////        fMaxX = max(fMaxX, vViewPoints[i].x);
////
////        fMinY = min(fMinY, vViewPoints[i].y);
////        fMaxY = max(fMaxY, vViewPoints[i].y);
////
////        fMinZ = min(fMinZ, vViewPoints[i].z);
////        fMaxZ = max(fMaxZ, vViewPoints[i].z);
////    }
////
////    _float fNear = fMinZ;
////    _float fFar = fMaxZ;
////
////    if (fNear > fFar)
////        swap(fNear, fFar);
////
////    m_fShadowLightFar = fFar;
////
////    return XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fNear, fFar);
////}
//
//void CShadow::Make_ShadowLight_SplitProj(const _float4* pWorldPoints)
//{
//    _float fCameraNear = m_pGameInstance->Get_CurrentCamera_Near();
//    _float fCameraFar = m_pGameInstance->Get_CurrentCamera_Far();
//
//    for (_uint i = 0; i <5; i++)
//    {
//        m_fSplitDistance[i] = Compute_Split_Distance(fCameraNear, fCameraFar, i, 4, 0.5f);
//    }
//
//    _float4 vViewPoints[8] = {};
//    
//    for (_uint j = 0; j < 8; j++)
//        XMStoreFloat4(&vViewPoints[j], XMVector3TransformCoord(XMLoadFloat4(&pWorldPoints[j]), m_CameraViewMatrix));
//
//    _float fSplitNear = {};
//    _float fSplitFar = {};
//    _float fNearRatio = {};
//    _float fFarRatio = {};
//
//    for(_uint k=0; k<4; k++)
//    {
//        fSplitNear = m_fSplitDistance[k];
//        fSplitFar = m_fSplitDistance[k + 1];
//
//        fNearRatio = (fSplitNear - fCameraNear) / (fCameraFar - fCameraNear);
//        fFarRatio = (fSplitFar - fCameraNear) / (fCameraFar - fCameraNear);
//
//        _float4 vSplitPoints[8] = {};
//
//        for (_uint h = 0; h < 4; h++)
//        {
//            XMStoreFloat4(&vSplitPoints[h], XMVectorLerp(XMLoadFloat4(&vViewPoints[h]), XMLoadFloat4(&vViewPoints[h + 4]), fNearRatio));
//            XMStoreFloat4(&vSplitPoints[h+4], XMVectorLerp(XMLoadFloat4(&vViewPoints[h]), XMLoadFloat4(&vViewPoints[h + 4]), fFarRatio));
//        }
//
//        _matrix ProjMatrix = Compute_ShadowLight_Proj(vSplitPoints, m_CameraViewMatrix);
//        XMStoreFloat4x4(&m_ShadowProjMatrices[k], ProjMatrix);
//    }
//}
//
//_matrix CShadow::Compute_ShadowLight_Proj(const _float4* pViewPoints, const _fmatrix ViewMatrixInv)
//{
//    _float4 vWorldPoints[8] = {};
//
//    for (_uint i = 0; i < 8; i++)
//        XMStoreFloat4(&vWorldPoints[i], XMVector3TransformCoord(XMLoadFloat4(&pViewPoints[i]), ViewMatrixInv));
//
//    _float4 vLightViewPoints[8] = {};
//
//    for (_uint j = 0; j < 8; j++)
//        XMStoreFloat4(&vLightViewPoints[j], XMVector3TransformCoord(XMLoadFloat4(&vWorldPoints[j]), XMLoadFloat4x4(&m_ShadowViewMatrix)));
//
//    _float fMinX = FLT_MAX, fMaxX = FLT_MAX * -1.f;
//    _float fMinY = FLT_MAX, fMaxY = FLT_MAX * -1.f;
//    _float fMinZ = FLT_MAX, fMaxZ = FLT_MAX * -1.f;
//
//    for (_uint k = 0; k < 8; k++)
//    {
//        fMinX = min(fMinX, vLightViewPoints[k].x);
//        fMaxX = max(fMaxX, vLightViewPoints[k].x);
//
//        fMinY = min(fMinY, vLightViewPoints[k].y);
//        fMaxY = max(fMaxY, vLightViewPoints[k].y);
//
//        fMinZ = min(fMinZ, vLightViewPoints[k].z);
//        fMaxZ = max(fMaxZ, vLightViewPoints[k].z);
//    }
//
//    _float fNear = fMinZ;
//    _float fFar = fMaxZ;
//
//    if (fNear > fFar)
//        swap(fNear, fFar);
//
//    return XMMatrixOrthographicOffCenterLH(fMinX, fMaxX, fMinY, fMaxY, fNear, fFar);
//}
//
//_float CShadow::Compute_Split_Distance(_float fCameraNear, _float fCameraFar, _uint iIndex, _uint iNumSplit, _float fLambda)
//{
//    _float fSplitDistance = {};
//    _float fLiearSplit = fCameraNear + (fCameraFar - fCameraNear) * (static_cast<_float>(iIndex) / iNumSplit);
//    _float fLogSplit = fCameraNear * powf(fCameraFar / fCameraNear, static_cast<_float>(iIndex) / iNumSplit);
//
//    fSplitDistance = (fLambda * fLogSplit) + ((1.f - fLambda) * fLiearSplit);
//
//    return fSplitDistance;
//}
//
////void CShadow::Make_SplitPoints(const _float4* pWorldPoints, _float fStartRatio, _float fEndRatio, _float4* pOut)
////{
////    _float4 vLinearPoints[8] = {};
////    _float4 vLogPoints[8] = {};
////    
////    // LEFT_TOP
////    XMStoreFloat4(&vLinearPoints[0], XMVectorLerp(XMLoadFloat4(&pWorldPoints[0]), XMLoadFloat4(&pWorldPoints[4]), fStartRatio));
////    XMStoreFloat4(&vLinearPoints[4], XMVectorLerp(XMLoadFloat4(&pWorldPoints[0]), XMLoadFloat4(&pWorldPoints[4]), fEndRatio));
////
////    // LEFT_BOTTOM
////    XMStoreFloat4(&vLinearPoints[3], XMVectorLerp(XMLoadFloat4(&pWorldPoints[3]), XMLoadFloat4(&pWorldPoints[7]), fStartRatio));
////    XMStoreFloat4(&vLinearPoints[7], XMVectorLerp(XMLoadFloat4(&pWorldPoints[3]), XMLoadFloat4(&pWorldPoints[7]), fEndRatio));
////
////    // RIGHT_TOP
////    XMStoreFloat4(&vLinearPoints[1], XMVectorLerp(XMLoadFloat4(&pWorldPoints[1]), XMLoadFloat4(&pWorldPoints[5]), fStartRatio));
////    XMStoreFloat4(&vLinearPoints[5], XMVectorLerp(XMLoadFloat4(&pWorldPoints[1]), XMLoadFloat4(&pWorldPoints[5]), fEndRatio));
////
////    // RIGHT_BOTTOM
////    XMStoreFloat4(&vLinearPoints[2], XMVectorLerp(XMLoadFloat4(&pWorldPoints[2]), XMLoadFloat4(&pWorldPoints[6]), fStartRatio));
////    XMStoreFloat4(&vLinearPoints[6], XMVectorLerp(XMLoadFloat4(&pWorldPoints[2]), XMLoadFloat4(&pWorldPoints[6]), fEndRatio));
////
////
////
////
////}

CShadow* CShadow::Create(_float fViewportWidth, _float fViewportHeight)
{
    CShadow* pInstance = new CShadow();
    if (FAILED(pInstance->Initialize(fViewportWidth, fViewportHeight)))
    {
        MSG_BOX(TEXT("Failed Created : CShadow"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CShadow::Free()
{
    __super::Free();
    Safe_Release(m_pGameInstance);
}
