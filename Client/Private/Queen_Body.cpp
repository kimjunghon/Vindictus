#include "ClientPch.h"
#include "Queen_Body.h"
#include "QueenAnimMachine.h"

CQueen_Body::CQueen_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBody{ pDevice, pDeviceContext }
{
}

CQueen_Body::CQueen_Body(const CQueen_Body& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CQueen_Body::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CQueen_Body::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	QUEEN_BODY_DESC* pDesc = static_cast<QUEEN_BODY_DESC*>(pArg);

	m_pIsBrokenLeg= pDesc->IsBrokenLeg;

	m_iMeshLegIndex = 2;
	
	m_pTransformCom->RotateQuaternion(XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(90.f), 0.f));

	return S_OK;
}

void CQueen_Body::Priority_Update(_float fTimeDelta)
{
}

void CQueen_Body::Update(_float fTimeDelta)
{
	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CQueen_Body::Late_Update(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixMultiply(m_pTransformCom->Get_WorldMatrix(), XMLoadFloat4x4(m_pPawnMatrix)));

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CQueen_Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (IsBroken(i))	
			continue;
		

		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);


		m_pModelCom->Render(i);
	}

	return S_OK;
}


_bool CQueen_Body::IsBroken(_uint iMeshIndex)
{
	if (*m_pIsBrokenLeg)
		return iMeshIndex == m_iMeshLegIndex;

	return false;
}

HRESULT CQueen_Body::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Queen_Body"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	ROOTMOTION_OPTION Option = {};
	Option.PositionX = true;
	Option.PositionY = false;
	Option.PositionZ = true;
	Option.Rotation = true;
	Option.RotationOnlyZ = true;

	m_pModelCom->Set_RootMotionOption(Option);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pAnimMachine = CQueenAnimMachine::Create();
	if (nullptr == m_pAnimMachine)
		return E_FAIL;

	return S_OK;
}

HRESULT CQueen_Body::Bind_ShaderResources()
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

CQueen_Body* CQueen_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CQueen_Body* pInstance = new CQueen_Body(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CQueen_Body"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CQueen_Body::Clone(void* pArg)
{
	CQueen_Body* pInstance = new CQueen_Body(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CQueen_Body"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQueen_Body::Free()
{
	__super::Free();

	Safe_Release(m_pAnimMachine);
}
