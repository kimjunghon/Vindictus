#include "ClientPch.h"
#include "Armor.h"
#include "Model.h"
#include "Bone.h"
#include "PlayerInstance.h"

CArmor::CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
	, m_pPlayerInstance { CPlayerInstance::GetInstance()}
{
	Safe_AddRef(m_pPlayerInstance);
}

CArmor::CArmor(const CArmor& Prototype)
	: CPawnObject { Prototype }
	, m_pPlayerInstance{ Prototype.m_pPlayerInstance }
{
	Safe_AddRef(m_pPlayerInstance);
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
	m_ArmorInfo.fHealth = 10.f;

	m_DyeingData[0] = DYEING_DATA(pDesc->Materials[0], RandomColor(pDesc->Materials[0]));
	m_DyeingData[1] = DYEING_DATA(pDesc->Materials[1], RandomColor(pDesc->Materials[1]));
	m_DyeingData[2] = DYEING_DATA(pDesc->Materials[2], RandomColor(pDesc->Materials[2]));

	if (FAILED(Ready_Components(pDesc->iArmorModelPrototypeLevelIndex, pDesc->strArmorModelPrototypeTag)))
		return E_FAIL;

	m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Update_PoseCombinedTransformationMatrix();

	return S_OK;
}

void CArmor::Priority_Update(_float fTimeDelta)
{
	//if (false == m_IsEquip)
	//	return;
}

void CArmor::Update(_float fTimeDelta)
{

}

void CArmor::Late_Update(_float fTimeDelta)
{
	//if (false == m_IsEquip)
	//	return;

	if (m_pModelCom[ENUM_CLASS(m_eArmorState)])
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
			MSG_BOX(TEXT("Failed Add RenderGroup"));

		m_pModelCom[ENUM_CLASS(m_eArmorState)]->Bind_ParentBone(m_ParentBones[ENUM_CLASS(m_eArmorState)]);
	}

}

HRESULT CArmor::Render()
{
	if (nullptr == m_pModelCom[ENUM_CLASS(m_eArmorState)])
		return S_OK;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom[ENUM_CLASS(m_eArmorState)]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{

		if (FAILED(m_pModelCom[ENUM_CLASS(m_eArmorState)]->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom[ENUM_CLASS(m_eArmorState)]->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i, m_ParentBones[ENUM_CLASS(m_eArmorState)])))
			return E_FAIL;

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::COLOR_MASKING));

		m_pModelCom[ENUM_CLASS(m_eArmorState)]->Render(i);
	}

	return S_OK;
}

HRESULT CArmor::RenderSlot(SLOT_RENDER_DESC SlotRenderDesc)
{
	if (FAILED(Bind_ShaderResources_RenderSlot(SlotRenderDesc)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		_bool hasNormal = {};

		m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Bind_Shader_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &hasNormal);

		m_pShaderCom->Bind_RawValue("g_HasNormal", &hasNormal, sizeof(_bool));

		if (FAILED(m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Bind_PoseBoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::COLOR_MASKING));

		m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
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

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_STATE::END); i++)
	{
		for (auto& pBone : m_ParentBones[i])
			Safe_Release(pBone);
		m_ParentBones[i].clear();
	
	}
	
	return S_OK;
}

void CArmor::DecreaseDurability(_float fDecreaseAmount)
{
	if (m_ArmorInfo.fHealth <= 0.f)
		return;

	m_ArmorInfo.fHealth -= fDecreaseAmount;

	if (m_ArmorInfo.fHealth <= 0.f)
	{
		m_eArmorState = ARMOR_STATE::BROKEN;
		if (m_eArmorType == ARMOR_TYPE::HEAD)
		{
			EVENT_BROKEN_HEAD Event;
			Event.IsBroken = true;
			m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::NONSTATIC), Event);
			return;
		}

		m_pPlayerInstance->ChangeStatus(m_ArmorInfo.fDefense * -1.f, STATUS_TYPE::DEF);
		m_pPlayerInstance->ChangeStatus(m_ArmorInfo.fBrokenDefense, STATUS_TYPE::DEF);
	}
}

void CArmor::Reset()
{
	m_ArmorInfo.fHealth = m_ArmorInfo.fFullHealth;
	
	if (m_eArmorState == ARMOR_STATE::BROKEN)
	{
		m_eArmorState = ARMOR_STATE::DEFAULT;

		m_pPlayerInstance->ChangeStatus(m_ArmorInfo.fDefense, STATUS_TYPE::DEF);
		m_pPlayerInstance->ChangeStatus(m_ArmorInfo.fBrokenDefense * -1.f, STATUS_TYPE::DEF);
	}
}

void CArmor::Dyeing(_uint iMaterialIndex, _float3 vColor)
{
	if (iMaterialIndex >= ENUM_CLASS(DYEING_PART::END))
		return;

	m_DyeingData[iMaterialIndex].second = vColor;
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_R", &(m_DyeingData[ENUM_CLASS(DYEING_PART::R)].second), sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_G", &(m_DyeingData[ENUM_CLASS(DYEING_PART::G)].second), sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_B", &(m_DyeingData[ENUM_CLASS(DYEING_PART::B)].second), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

HRESULT CArmor::Bind_ShaderResources_RenderSlot(SLOT_RENDER_DESC SlotRenderDesc)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &SlotRenderDesc.WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &SlotRenderDesc.ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &SlotRenderDesc.ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_R", &(m_DyeingData[ENUM_CLASS(DYEING_PART::R)].second), sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_G", &(m_DyeingData[ENUM_CLASS(DYEING_PART::G)].second), sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_B", &(m_DyeingData[ENUM_CLASS(DYEING_PART::B)].second), sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}


HRESULT CArmor::Bind_ParentBones(CModel* pParentModelCom)
{
	for (_uint i = 0; i < ENUM_CLASS(ARMOR_STATE::END); i++)
	{
		if (false == m_ParentBones[i].empty())
		{
			for (auto& pBone : m_ParentBones[i])
				Safe_Release(pBone);
			m_ParentBones[i].clear();
		}
	}

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
				m_ParentBones[ENUM_CLASS(ARMOR_STATE::DEFAULT)].push_back(ParentBones[j]);
				Safe_AddRef(ParentBones[j]);
				break;
			}
		}

		if(false == IsCompare)
		{
			m_ParentBones[ENUM_CLASS(ARMOR_STATE::DEFAULT)].push_back(ArmorBones[i]);
			Safe_AddRef(ArmorBones[i]);
		}
	}
	if (m_pModelCom[ENUM_CLASS(ARMOR_STATE::BROKEN)])
	{
		vector<CBone*> BrokenArmorBones = m_pModelCom[ENUM_CLASS(ARMOR_STATE::BROKEN)]->Get_Bones();

		for (_uint i = 0; i < BrokenArmorBones.size(); i++)
		{
			_bool IsCompare = false;

			for (_uint j = 0; j < ParentBones.size(); j++)
			{
				if (ParentBones[j]->Compare_Name(BrokenArmorBones[i]->Get_Name()))
				{
					IsCompare = true;
					m_ParentBones[ENUM_CLASS(ARMOR_STATE::BROKEN)].push_back(ParentBones[j]);
					Safe_AddRef(ParentBones[j]);
					break;
				}
			}

			if (false == IsCompare)
			{
				m_ParentBones[ENUM_CLASS(ARMOR_STATE::BROKEN)].push_back(BrokenArmorBones[i]);
				Safe_AddRef(BrokenArmorBones[i]);
			}
		}
	}

	return S_OK;
}

_matrix CArmor::Compute_OffsetMatrix()
{
	MODEL_BOUNDING Bounding = m_pModelCom[ENUM_CLASS(ARMOR_STATE::DEFAULT)]->Get_ModelBounding();

	_float fCenterX = (Bounding.vMinPosition.x + Bounding.vMaxPosition.x) * 0.5f;
	_float fCenterY = (Bounding.vMinPosition.y + Bounding.vMaxPosition.y) * 0.5f;
	_float fCenterZ = (Bounding.vMinPosition.z + Bounding.vMaxPosition.z) * 0.5f;

	_matrix OffsetMatrix = XMMatrixTranslation(fCenterX * -1.f, fCenterZ , fCenterY * -1.f);

	return OffsetMatrix;
}

_float3 CArmor::RandomColor(DYEING_MATERIAL eMaterial)
{
	_float3 vColor = {};

	_float fMin = {};
	_float fMax = 1.f;

	switch (eMaterial)
	{
	case DYEING_MATERIAL::ARMOR_METAL:
		fMin = 0.7f;
		break;
	case DYEING_MATERIAL::LEATHER:
		fMin = 0.4f;
		break;
	case DYEING_MATERIAL::CLOTH:
		fMin = 0.f;
		break;
	}

	vColor.x = m_pGameInstance->Rand(fMin, fMax);
	vColor.y = m_pGameInstance->Rand(fMin, fMax);
	vColor.z = m_pGameInstance->Rand(fMin, fMax);

	return vColor;
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

	Safe_Release(m_pPlayerInstance);

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_STATE::END); i++)
	{
		for (auto& pBone : m_ParentBones[i])
			Safe_Release(pBone);
		m_ParentBones[i].clear();
	}

	for (_uint i = 0; i < ENUM_CLASS(ARMOR_STATE::END); i++)
		Safe_Release(m_pModelCom[i]);

	Safe_Release(m_pShaderCom);
}
