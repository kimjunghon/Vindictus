#include "ClientPch.h"
#include "Cat_Body.h"
#include "CatAnimMachine.h"

CCat_Body::CCat_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBody {pDevice, pDeviceContext}
{
}

CCat_Body::CCat_Body(const CCat_Body& Prototype)
	: CBody { Prototype }
{
}

HRESULT CCat_Body::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CCat_Body::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);

	return S_OK;
}

void CCat_Body::Priority_Update(_float fTimeDelta)
{
}

void CCat_Body::Update(_float fTimeDelta)
{
	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CCat_Body::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CCat_Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		_bool hasNormal = {};

		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &hasNormal);

		m_pShaderCom->Bind_RawValue("g_HasNormal", &hasNormal, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::DEFAULT));

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CCat_Body::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_Cat_Body"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	ROOTMOTION_OPTION Option = {};
	Option.PositionX = false;
	Option.PositionY = false;
	Option.PositionZ = false;
	Option.Rotation = false;
	Option.RotationOnlyZ = false;

	m_pModelCom->Set_RootMotionOption(Option);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pAnimMachine = CCatAnimMachine::Create();
	if (nullptr == m_pAnimMachine)
		return E_FAIL;

	return S_OK;
}

HRESULT CCat_Body::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pPawnMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CCat_Body* CCat_Body::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCat_Body* pInstance = new CCat_Body(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CCat_Body"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCat_Body::Clone(void* pArg)
{
	CCat_Body* pInstance = new CCat_Body(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CCat_Body"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCat_Body::Free()
{
	__super::Free();

	Safe_Release(m_pAnimMachine);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}