#include "ClientPch.h"
#include "MapObject.h"

CMapObject::CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
}

CMapObject::CMapObject(const CMapObject& Prototype)
	: CGameObject{ Prototype }
{
}

HRESULT CMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAP_OBJECT_DESC* pDesc = static_cast<MAP_OBJECT_DESC*>(pArg);

	m_pTransformCom->Set_WorldMatrix(pDesc->WorldMatrix);
	m_IsNormal = pDesc->IsNormal;
	m_IsSpecular = pDesc->IsSpecular;
	m_IsAmbient = pDesc->IsAmbient;

	if (FAILED(Ready_Components(pDesc->iModelLevel, pDesc->strModelTag)))
		return E_FAIL;

	return S_OK;
}

void CMapObject::Priority_Update(_float fTimeDelta)
{
}

void CMapObject::Update(_float fTimeDelta)
{
}

void CMapObject::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CMapObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0);
		
		_bool hasNormal = {};

		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &hasNormal);
		
		m_pShaderCom->Bind_RawValue("g_HasNormal", &hasNormal, sizeof(_bool));

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXMESH::MAP_OBJECT));

		m_pModelCom->Render(i);

		m_pShaderCom->Bind_SPV("g_DiffuseTexture", nullptr);
	}

	return S_OK;
}

HRESULT CMapObject::Ready_Components(_uint iModelLevel, const _wstring& strModelTag)
{
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(iModelLevel, strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsNormal", &m_IsNormal, sizeof(_bool))))
		return E_FAIL;

	return S_OK;
}

CMapObject* CMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMapObject* pInstance = new CMapObject(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CMapObject"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMapObject::Clone(void* pArg)
{
	CMapObject* pInstance = new CMapObject(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CMapObject"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
