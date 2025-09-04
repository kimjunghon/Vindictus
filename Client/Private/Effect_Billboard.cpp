#include "ClientPch.h"
#include "Effect_Billboard.h"

CEffect_Billboard::CEffect_Billboard(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CEffect { pDevice, pDeviceContext }
{
}

CEffect_Billboard::CEffect_Billboard(const CEffect_Billboard& Prototype)
    : CEffect { Prototype }
{
}

HRESULT CEffect_Billboard::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Billboard::Initialize(void* pArg)
{
	

    return S_OK;
}

void CEffect_Billboard::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Billboard::Update(_float fTimeDelta)
{
}

void CEffect_Billboard::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect_Billboard::Render()
{
    return S_OK;
}

HRESULT CEffect_Billboard::Spawn(void* pArg)
{
    return S_OK;
}

HRESULT CEffect_Billboard::Ready_Component(const _wstring& EffectName)
{
    _wstring strTextureTag = TEXT("Prototype_Component_Texture_") + EffectName;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    _wstring strVIBufferTag = TEXT("Prototype_Component_EffectBuffer_") + EffectName;
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), strVIBufferTag, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParitlce"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Billboard* CEffect_Billboard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffect_Billboard* pInstance = new CEffect_Billboard(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created CEffect_Billboard"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Billboard::Clone(void* pArg)
{
	CEffect_Billboard* pInstance = new CEffect_Billboard(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned CEffect_Billboard"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Billboard::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
