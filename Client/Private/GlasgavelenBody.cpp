#include "ClientPch.h"
#include "GlasgavelenBody.h"

CGlasgavelenBody::CGlasgavelenBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBody { pDevice, pDeviceContext }
{
}

CGlasgavelenBody::CGlasgavelenBody(const CGlasgavelenBody& Prototype)
	: CBody { Prototype }
{
}

HRESULT CGlasgavelenBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGlasgavelenBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelenBody::Priority_Update(_float fTimeDelta)
{

}

void CGlasgavelenBody::Update(_float fTimeDelta)
{
	ANIM_DATA data = {};
	data.strAnimKey = "Idle";
	data.IsLoop = true;
	data.vRange = _float2(1.f, 1.f);
	data.fAnimSpeed = 2.f;

	m_pModelCom->Set_Animation(data);
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CGlasgavelenBody::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CGlasgavelenBody::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);


		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CGlasgavelenBody::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Glasgavelen"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Glasgavelen_Broken"),
		TEXT("Com_Broken_Model"), reinterpret_cast<CComponent**>(&m_pBrokenModelCom))))
		return E_FAIL;


	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelenBody::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pPawnMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(TEXT("DIRECTONAL"));
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CGlasgavelenBody* CGlasgavelenBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGlasgavelenBody* pInstance = new CGlasgavelenBody(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CGlasgavelenBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGlasgavelenBody::Clone(void* pArg)
{
	CGlasgavelenBody* pInstance = new CGlasgavelenBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CGlasgavelenBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGlasgavelenBody::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}