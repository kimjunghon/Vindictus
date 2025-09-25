#include "ClientPch.h"
#include "DamageFont.h"

CDamageFont::CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext }
{
}

CDamageFont::CDamageFont(const CDamageFont& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CDamageFont::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDamageFont::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDamageFont::Priority_Update(_float fTimeDelta)
{
}

void CDamageFont::Update(_float fTimeDelta)
{
}

void CDamageFont::Late_Update(_float fTimeDelta)
{
}

HRESULT CDamageFont::Render()
{
    return S_OK;
}

HRESULT CDamageFont::Spawn(void* pArg)
{
    DAMAGE_DESC* pDesc = static_cast<DAMAGE_DESC*>(pArg);

    _vector vPosition = pDesc->vPosition;

    _matrix ViewMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
    _matrix ProjMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
    
    vPosition = XMVector4Transform(vPosition, ViewMatrix);
    vPosition = XMVector4Transform(vPosition, ProjMatrix);
    
    m_fX = XMVectorGetX(vPosition) * (g_iWinSizeX << 1);
    m_fY = XMVectorGetY(vPosition) * (g_iWinSizeY << 1);
    
    m_iDamage = pDesc->iDamage;

    return S_OK;
}

HRESULT CDamageFont::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DamageFont"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDamageFont::Bind_ShaderResources()
{
    __super::Begin();

    if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DAMAGE_FONT));

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

CDamageFont* CDamageFont::Created(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CDamageFont* pInstance = new CDamageFont(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CDamageFont"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDamageFont::Clone(void* pArg)
{
    CDamageFont* pInstance = new CDamageFont(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CDamageFont"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDamageFont::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
