#include "ClientPch.h"
#include "GlasgavelenSword.h"
#include "Bone.h"

CGlasgavelenSword::CGlasgavelenSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
{
}

CGlasgavelenSword::CGlasgavelenSword(const CGlasgavelenSword& Prototype)
	: CPawnObject { Prototype }
{
}

HRESULT CGlasgavelenSword::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGlasgavelenSword::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CGlasgavelenSword::Priority_Update(_float fTimeDelta)
{
}

void CGlasgavelenSword::Update(_float fTimeDelta)
{
}

void CGlasgavelenSword::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);

	m_pModelCom->Bind_ParentBone(m_ParentBones);
}

HRESULT CGlasgavelenSword::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i, m_ParentBones)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CGlasgavelenSword::Ready_Components()
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_GavelneSword"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlasgavelenSword::Bind_ShaderResources()
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

HRESULT CGlasgavelenSword::Bind_ParentBones(CModel* pParentModelCom)
{
	if (false == m_ParentBones.empty())
	{
		for (auto& pBone : m_ParentBones)
			Safe_Release(pBone);

		m_ParentBones.clear();
	}

	vector<CBone*> SwordBones = m_pModelCom->Get_Bones();
	vector<CBone*> ParentBones = pParentModelCom->Get_Bones();

	for (_uint i = 0; i < SwordBones.size(); i++)
	{
		_bool IsCompare = false;

		for (_uint j = 0; j < ParentBones.size(); j++)
		{
			if (ParentBones[j]->Compare_Name(SwordBones[i]->Get_Name()))
			{
				IsCompare = true;
				m_ParentBones.push_back(ParentBones[j]);
				Safe_AddRef(ParentBones[j]);
				break;
			}
		}

		if (false == IsCompare)
		{
			m_ParentBones.push_back(SwordBones[i]);
			Safe_AddRef(SwordBones[i]);
		}
	}

	return S_OK;
}

CGlasgavelenSword* CGlasgavelenSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGlasgavelenSword* pInstance = new CGlasgavelenSword(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CGlasgavelenSword"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGlasgavelenSword::Clone(void* pArg)
{
	CGlasgavelenSword* pInstance = new CGlasgavelenSword(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CGlasgavelenSword"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGlasgavelenSword::Free()
{
	__super::Free();

	for (auto& pBone : m_ParentBones)
		Safe_Release(pBone);

	m_ParentBones.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
