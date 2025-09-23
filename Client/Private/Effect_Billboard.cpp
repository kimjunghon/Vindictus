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
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Billboard::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Billboard::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Billboard::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Update(fTimeDelta, &m_IsFinished);

	if (m_IsFinished)
	{
		ReturnToPool();
	}
}

void CEffect_Billboard::Late_Update(_float fTimeDelta)
{
	if(m_IsEmissive)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::EMISSIVE, this);
	else
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);

	m_pVIBufferCom->Sort(m_pTransformCom->Get_WorldMatrix());
}

HRESULT CEffect_Billboard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassIndex);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}


HRESULT CEffect_Billboard::Spawn(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

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

HRESULT CEffect_Billboard::Ready_Component()
{
    _wstring strTextureTag = TEXT("Prototype_Component_Texture_") + m_strEffectName;
    if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, strTextureTag, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    _wstring strVIBufferTag = TEXT("Prototype_Component_EffectBuffer_") + m_strEffectName;
    if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, strVIBufferTag, TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_PointParitlce"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Billboard::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Shader_Color(m_pShaderCom, "g_vSourceColor")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsEmissive", &m_IsEmissive, sizeof(_bool))))
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
