#include "ClientPch.h"
#include "Armor.h"
#include "Model.h"
#include "Bone.h"


CArmor::CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
{
}

CArmor::CArmor(const CArmor& Prototype)
	: CPawnObject { Prototype }
{
}

HRESULT CArmor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArmor::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ARMOR_DESC* pDesc = static_cast<ARMOR_DESC*>(pArg);

	m_eArmorType = pDesc->eArmorType;
	m_ArmorInfo = pDesc->ArmorInfo;

	if (FAILED(Ready_Components(pDesc->iArmorModelPrototypeLevelIndex, pDesc->strArmorModelPrototypeTag)))
		return E_FAIL;

	return S_OK;
}

void CArmor::Priority_Update(_float fTimeDelta)
{
	if (false == m_IsEquip)
		return;
}

void CArmor::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_1))
	{
		if (m_eArmorType == ARMOR_TYPE::HEAD)
			m_ArmorInfo.fHealth = 0.f;
	}

	if (m_pGameInstance->Get_KeyDown(DIK_2))
	{
		if (m_eArmorType == ARMOR_TYPE::UPPER)
			m_ArmorInfo.fHealth = 0.f;
	}
	if (m_pGameInstance->Get_KeyDown(DIK_3))
	{
		if (m_eArmorType == ARMOR_TYPE::LOWER)
			m_ArmorInfo.fHealth = 0.f;
	}
	if (m_pGameInstance->Get_KeyDown(DIK_4))
	{
		if (m_eArmorType == ARMOR_TYPE::HAND)
			m_ArmorInfo.fHealth = 0.f;
	}
	if (m_pGameInstance->Get_KeyDown(DIK_5))
	{
		if (m_eArmorType == ARMOR_TYPE::FOOT)
			m_ArmorInfo.fHealth = 0.f;
	}

	if (false == m_IsEquip)
		return;
}

void CArmor::Late_Update(_float fTimeDelta)
{
	if (false == m_IsEquip)
		return;

	if (m_ArmorInfo.fHealth <= 0.f)
	{
		m_eArmorState = ARMOR_STATE::BROKEN;		
		if (m_eArmorType == ARMOR_TYPE::HEAD)
		{
			EVENT_BROKEN_HEAD Event;
			m_pGameInstance->Publish(ENUM_CLASS(LEVEL::GAMEPLAY), Event);
			return;
		}
	}

	if (m_pModelCom[ENUM_CLASS(m_eArmorState)])
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
			MSG_BOX(TEXT("Failed Add RenderGroup"));
	}

}

HRESULT CArmor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom[ENUM_CLASS(m_eArmorState)]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom[ENUM_CLASS(m_eArmorState)]->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom[ENUM_CLASS(m_eArmorState)]->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i, m_ParentBones)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom[ENUM_CLASS(m_eArmorState)]->Render(i);
	}

	return S_OK;
}

HRESULT CArmor::Equip(const _float4x4* pPawnMatrix, CModel* pParentModelCom)
{
	if (FAILED(Bind_ParentBones(pParentModelCom)))
		return E_FAIL;

	m_pPawnMatrix = pPawnMatrix;

	m_IsEquip = true;

	return S_OK;
}

HRESULT CArmor::UnEquip()
{
	m_pPawnMatrix = nullptr;

	for (auto& pBone : m_ParentBones)
		Safe_Release(pBone);

	m_ParentBones.clear();

	return S_OK;
}

HRESULT CArmor::Ready_Components(_uint iArmorNodelPrototypeLevelIndex, const _wstring& strArmorModelPrototypeTag)
{
	if(FAILED(CGameObject::Add_Component(iArmorNodelPrototypeLevelIndex, strArmorModelPrototypeTag,
		TEXT("Com_Default_Model"), reinterpret_cast<CComponent**>(&m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]))))
		return E_FAIL;

	if (m_eArmorType != ARMOR_TYPE::HEAD)
	{
		_wstring strArmorBrokenModelTag = strArmorModelPrototypeTag + TEXT("_Broken");

		if (FAILED(CGameObject::Add_Component(iArmorNodelPrototypeLevelIndex, strArmorBrokenModelTag,
			TEXT("Com_Broken_Model"), reinterpret_cast<CComponent**>(&m_pModelCom[ENUM_CLASS(ARMOR_STATE::BROKEN)]))))
			return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmor::Bind_ShaderResources()
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

HRESULT CArmor::Bind_ParentBones(CModel* pParentModelCom)
{
	vector<CBone*> ArmorBones = m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Get_Bones();
	vector<CBone*> ParentBones = pParentModelCom->Get_Bones();

	for (_uint i = 0; i < ArmorBones.size(); i++)
	{
		_bool IsCompare = false;

		for (_uint j = 0; j < ParentBones.size(); j++)
		{
			if (ParentBones[j]->Compare_Name(ArmorBones[i]->Get_Name()))
			{
				IsCompare = true;
				m_ParentBones.push_back(ParentBones[j]);
				Safe_AddRef(ParentBones[j]);
				break;
			}
		}

		if(false == IsCompare)
		{
			m_ParentBones.push_back(ArmorBones[i]);
			Safe_AddRef(ArmorBones[i]);
		}
	}

	return S_OK;
}

CArmor* CArmor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CArmor* pInstance = new CArmor(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CArmor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CArmor::Clone(void* pArg)
{
	CArmor* pInstance = new CArmor(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CArmor"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CArmor::Free()
{
	__super::Free();

	for (auto& pBone : m_ParentBones)
		Safe_Release(pBone);

	m_ParentBones.clear();

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_STATE::END); i++)
		Safe_Release(m_pModelCom[i]);

	Safe_Release(m_pShaderCom);
}
