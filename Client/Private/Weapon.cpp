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

	if (FAILED(Ready_Components(pDesc->iWeaponModelPrototypeLevelIndex, pDesc->strWeaponModelPrototypeTag)))
		return E_FAIL;

	m_pTransformCom->RotateQuaternion(XMLoadFloat4(&pDesc->vRotationQuaternion));

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

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}

	return S_OK;
}

HRESULT CWeapon::RenderSlot(SLOT_RENDER_DESC SlotRenderDesc)
{
	if (FAILED(Bind_ShaderResources_RenderSlot(SlotRenderDesc.ViewMatrix, SlotRenderDesc.ViewMatrix)))
		return E_FAIL;


	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}


	return S_OK;
}

HRESULT CWeapon::Equip(const _float4x4* pPawnMatrix, const _float4x4* pSocketMatrix)
{
	if (nullptr == pSocketMatrix)
		return E_FAIL;

	m_pPawnMatrix = pPawnMatrix;
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

HRESULT CWeapon::Bind_ShaderResources_RenderSlot(_float4x4 ViewMatrix, _float4x4 ProjMatirx)
{
	_float4x4 WorldMatrix = {};
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatirx)))
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

CWeapon* CWeapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

CGameObject* CWeapon::Clone(void* pArg)
{
	return nullptr;
}

void CWeapon::Free()
{
}
