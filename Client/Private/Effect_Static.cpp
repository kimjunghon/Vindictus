#include "ClientPch.h"
#include "Effect_Static.h"

CEffect_Static::CEffect_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CEffect { pDevice, pDeviceContext }
{
}

CEffect_Static::CEffect_Static(const CEffect_Static& Prototype)
	: CEffect { Prototype }
{
}

HRESULT CEffect_Static::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Static::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Static::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Static::Update(_float fTimeDelta)
{
	m_pVIBufferCom->Update(fTimeDelta, &m_IsFinished);

	if (m_IsFinished)
	{
		ReturnToPool();
	}
}

void CEffect_Static::Late_Update(_float fTimeDelta)
{
	if (m_IsEmissive)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::EMISSIVE, this);
	else
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CEffect_Static::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassIndex);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Static::Spawn(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	EFFECT_SPAWN_DESC* pDesc = static_cast<EFFECT_SPAWN_DESC*>(pArg);

	_matrix CurrentWorldMatrix = pDesc->SpawnWorldMatrix;

	m_IsEmissive = pDesc->IsEmissive;

	m_pTransformCom->Set_WorldMatrix(CurrentWorldMatrix);

	m_IsActive = true;

	m_IsFinished = false;

	m_pVIBufferCom->Reset();
	
	return S_OK;
}

void CEffect_Static::ReturnToPool()
{
	m_IsActive = false;

	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());

	m_pPool_Instance->ReturnPool(m_strEffectName, this);
}

HRESULT CEffect_Static::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_EffectBuffer_") + m_strEffectName,
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_") + m_strEffectName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxInstance_Paritlce"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Static::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Shader_Color(m_pShaderCom, "g_vSourceColor")))
		return E_FAIL;

	return S_OK;
}

CEffect_Static* CEffect_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEffect_Static* pInstance = new CEffect_Static(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CEffect_Static"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Static::Clone(void* pArg)
{
	CEffect_Static* pInstance = new CEffect_Static(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CEffect_Static"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Static::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
