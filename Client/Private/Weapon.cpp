#include "ClientPch.h"
#include "Weapon.h"

CWeapon::CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
{
}

CWeapon::CWeapon(const CWeapon& Prototype)
	: CPawnObject { Prototype }
{
}

HRESULT CWeapon::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CWeapon::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(pArg);

	m_eWeaponType = pDesc->eWeaponType;
	m_WeaponInfo = pDesc->WeaponInfo;

	m_DyeingData[0] = DYEING_DATA(pDesc->Materials[0], RandomColor(pDesc->Materials[0]));
	m_DyeingData[1] = DYEING_DATA(pDesc->Materials[1], RandomColor(pDesc->Materials[1]));
	m_DyeingData[2] = DYEING_DATA(pDesc->Materials[2], RandomColor(pDesc->Materials[2]));

	if (FAILED(Ready_Components(pDesc->iWeaponModelPrototypeLevelIndex, pDesc->strWeaponModelPrototypeTag)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vOffsetPosition);
	m_pTransformCom->RotateQuaternion(pDesc->vRotationQuaternion);

	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixIdentity());

	return S_OK;
}

void CWeapon::Priority_Update(_float fTimeDelta)
{
	if (false == m_IsEquip)
		return;
}

void CWeapon::Update(_float fTimeDelta)
{
	if (false == m_IsEquip)
		return;
}

void CWeapon::Late_Update(_float fTimeDelta)
{
	if (false == m_IsEquip)
		return;

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixMultiply(XMMatrixMultiply(m_pTransformCom->Get_WorldMatrix(), SocketMatrix), XMLoadFloat4x4(m_pPawnMatrix)));

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this);
}

HRESULT CWeapon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXMESH::COLOR_MASKING));

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}

	return S_OK;
}

HRESULT CWeapon::RenderSlot(SLOT_RENDER_DESC SlotRenderDesc)
{
	if (FAILED(Bind_ShaderResources_RenderSlot(SlotRenderDesc)))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		_bool hasNormal = {};

		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &hasNormal);

		m_pShaderCom->Bind_RawValue("g_HasNormal", &hasNormal, sizeof(_bool));

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXMESH::COLOR_MASKING));

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}

	return S_OK;
}

HRESULT CWeapon::Equip(const _float4x4* pParentMatrix, const _float4x4* pSocketMatrix, void* pDesc)
{
	if (nullptr == pSocketMatrix)
		return E_FAIL;

	m_pPawnMatrix = pParentMatrix;
	m_pSocketMatrix = pSocketMatrix;

	m_IsEquip = true;

	return S_OK;
}

HRESULT CWeapon::UnEquip()
{
	m_pPawnMatrix = nullptr;
	m_pSocketMatrix = nullptr;

	m_IsEquip = false;

	return S_OK;
}

void CWeapon::Dyeing(_uint iMaterialIndex, _float3 vColor)
{
	if (iMaterialIndex >= ENUM_CLASS(DYEING_PART::END))
		return;

	m_DyeingData[iMaterialIndex].second = vColor;
}

HRESULT CWeapon::Ready_Components(_uint iWeaponModelPrototypeLevelIndex, const _wstring& strWeaponModelPrototypeTag)
{
	if (FAILED(__super::Add_Component(iWeaponModelPrototypeLevelIndex, strWeaponModelPrototypeTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedMatrix)))
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

HRESULT CWeapon::Bind_ShaderResources_RenderSlot(SLOT_RENDER_DESC SlotRenderDesc)
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

_float3 CWeapon::RandomColor(DYEING_MATERIAL eMaterial)
{
	_float3 vColor = {};

	_float fMin = {};
	_float fMax = 1.f;

	switch (eMaterial)
	{
	case DYEING_MATERIAL::WEAPON_METAL:
		fMin = 0.7f;
		break;
	case DYEING_MATERIAL::LEATHER:
		fMin = 0.4f;
		break;
	}

	vColor.x = m_pGameInstance->Rand(fMin, fMax);
	vColor.y = m_pGameInstance->Rand(fMin, fMax);
	vColor.z = m_pGameInstance->Rand(fMin, fMax);

	return vColor;
}

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created CWeapon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	CWeapon* pInstance = new CWeapon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned CWeapon"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
