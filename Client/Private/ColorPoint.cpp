#include "ClientPch.h"
#include "ColorPoint.h"

CColorPoint::CColorPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext }
{
}

CColorPoint::CColorPoint(const CColorPoint& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CColorPoint::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CColorPoint::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CColorPoint::Priority_Update(_float fTimeDelta)
{
}

void CColorPoint::Update(_float fTimeDelta)
{
}

void CColorPoint::Late_Update(_float fTimeDelta)
{

}

HRESULT CColorPoint::Render()
{
    __super::Begin();

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::COLOR));

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CColorPoint::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CColorPoint::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vDyeingColor, sizeof(_float3))))
        return E_FAIL;

    return S_OK;
}

CColorPoint* CColorPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CColorPoint* pInstance = new CColorPoint(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CColorPoint"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CColorPoint::Clone(void* pArg)
{
    CColorPoint* pInstance = new CColorPoint(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CColorPoint"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CColorPoint::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
