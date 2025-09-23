#include "ClientPch.h"
#include "Effect_Distortion.h"

CEffect_Distortion::CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect { pDevice, pDeviceContext }
{
}

CEffect_Distortion::CEffect_Distortion(const CEffect_Distortion& Prototype)
    : CEffect { Prototype }
{
}

HRESULT CEffect_Distortion::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Distortion::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;
    
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    DISTORTION_DESC* pDesc = static_cast<DISTORTION_DESC*>(pArg);
    m_IsMasking = pDesc->Is_Masking;

    if (m_IsMasking)
    {
        if (FAILED(Ready_MaskTexture(pDesc->strMaskTextureName)))
            return E_FAIL;
    }

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CEffect_Distortion::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Distortion::Update(_float fTimeDelta)
{
    m_pVIBufferCom->Update(fTimeDelta, &m_IsFinished);
}

void CEffect_Distortion::Late_Update(_float fTimeDelta)
{
    if (m_IsFinished)
    {
        ReturnToPool();
        return;
    }

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::DISTORTION, this)))
        return;
}

HRESULT CEffect_Distortion::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(m_iPassIndex);

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CEffect_Distortion::Spawn(void* pArg)
{
    EFFECT_SPAWN_DESC* pDesc = static_cast<EFFECT_SPAWN_DESC*>(pArg);

    _matrix* pWorldMatrix = static_cast<_matrix*>(pArg);

    _matrix CurrentWorldMatrix = pDesc->SpawnWorldMatrix;

    m_pTransformCom->Set_WorldMatrix(CurrentWorldMatrix);

    m_IsEmissive = pDesc->IsEmissive;

    m_IsActive = true;

    m_IsFinished = false;

    m_pVIBufferCom->Reset();

    return S_OK;
}

HRESULT CEffect_Distortion::Ready_Component()
{
    _wstring strTextureTag = TEXT("Prototype_Component_Texture_") + m_strEffectName;
    if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDistortionTextureCom))))
        return E_FAIL;

    _wstring strVIBufferTag = TEXT("Prototype_Component_EffectBuffer_") + m_strEffectName;
    if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, strVIBufferTag, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Distortion"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Distortion::Ready_MaskTexture(const _wstring& strMaskTextureName)
{
    _wstring strMaskTextureTag = TEXT("Prototype_Component_Mask_Texture_") + strMaskTextureName;
    if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, strMaskTextureTag, TEXT("Com_MaskTexture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CEffect_Distortion::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pDistortionTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_DistortionTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;

    if (m_IsMasking)
    {
        if (FAILED(m_pMaskTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_MaskTexture", 0)))
            return E_FAIL;
    }

    return S_OK;
}

CEffect_Distortion* CEffect_Distortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEffect_Distortion* pInstance = new CEffect_Distortion(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CEffect_Distortion"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Distortion::Clone(void* pArg)
{
    CEffect_Distortion* pInstance = new CEffect_Distortion(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CEffect_Distortion"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Distortion::Free()
{
    __super::Free();

    Safe_Release(m_pDistortionTextureCom);

    if(m_IsMasking)
        Safe_Release(m_pMaskTextureCom);
    
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
