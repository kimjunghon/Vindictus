#include "ClientPch.h"
#include "PuppyBody.h"
#include "PuppyAnimMachine.h"

CPuppyBody::CPuppyBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBody { pDevice, pDeviceContext }
{
}

CPuppyBody::CPuppyBody(const CPuppyBody& Prototype)
	: CBody { Prototype }
{
}

HRESULT CPuppyBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPuppyBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PUPPY_BODY_DESC* pDesc = static_cast<PUPPY_BODY_DESC*>(pArg);

	if (FAILED(Ready_Components(pDesc->iPuppyTypeIndex)))
		return E_FAIL;

	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);


	return S_OK;
}

void CPuppyBody::Priority_Update(_float fTimeDelta)
{
}

void CPuppyBody::Update(_float fTimeDelta)
{
	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);

	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPuppyBody::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CPuppyBody::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

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

HRESULT CPuppyBody::Ready_Components(_uint iPuppyType)
{
	_wstring strModelTag = {};

	switch (static_cast<PUPPY_TYPE>(iPuppyType))
	{
	case PUPPY_TYPE::GRAY:
		strModelTag = TEXT("Prototype_Component_Model_Puppy_Gray");
		break;
	case PUPPY_TYPE::WHITE:
		strModelTag = TEXT("Prototype_Component_Model_Puppy_White");
		break;
	case PUPPY_TYPE::ARMOR:
		strModelTag = TEXT("Prototype_Component_Model_Puppy_Armor");
		break;
	}

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::TOWN), strModelTag,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	ROOTMOTION_OPTION Option = {};
	Option.PositionX = true;
	Option.PositionY = false;
	Option.PositionZ = true;
	Option.Rotation = false;
	Option.RotationOnlyZ = false;

	m_pModelCom->Set_RootMotionOption(Option);

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	m_pAnimMachine = CPuppyAnimMachine::Create();
	if (nullptr == m_pAnimMachine)
		return E_FAIL;


	return S_OK;
}

HRESULT CPuppyBody::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pPawnMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

CPuppyBody* CPuppyBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPuppyBody* pInstance = new CPuppyBody(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPuppyBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPuppyBody::Clone(void* pArg)
{
	CPuppyBody* pInstance = new CPuppyBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPuppyBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPuppyBody::Free()
{
	__super::Free();

	Safe_Release(m_pAnimMachine);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}