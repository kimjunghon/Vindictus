#include "ClientPch.h"
#include "PlayerBody.h"
#include "PlayerAnimMachine.h"

CPlayerBody::CPlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBody{ pDevice, pDeviceContext }
{
}

CPlayerBody::CPlayerBody(const CPlayerBody& Prototype)
	: CBody{ Prototype }
{
}

HRESULT CPlayerBody::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayerBody::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_BodyColors()))
		return E_FAIL;
	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);

	m_pGameInstance->Subscribe<EVENT_BROKEN_HEAD>(ENUM_CLASS(EVENT_TYPE::NONSTATIC), [&](const EVENT_BROKEN_HEAD& Event) {
		this->IsHair(Event.IsBroken); });

	return S_OK;
}

void CPlayerBody::Priority_Update(_float fTimeDelta)
{
}

void CPlayerBody::Update(_float fTimeDelta)
{
	m_pAnimMachine->Set_Animation(m_pModelCom, *m_pStateFlag);
		
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CPlayerBody::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::SHADOW, this)))
		return;
	if(FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CPlayerBody::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (false == m_IsHair && i == PART::HAIR)
			continue;

		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		_bool hasNormal = {};

		m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS, 0, &hasNormal);

		m_pShaderCom->Bind_RawValue("g_HasNormal", &hasNormal, sizeof(_bool));

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		if(i == PART::HAIR || i == PART::LOWER || i == PART::UPPER)
		{
			if (FAILED(Bind_Color(i)))
				return E_FAIL;

			m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::COLOR_MASKING));
		}
		else
			m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::DEFAULT));

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayerBody::Render_Shadow()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pPawnMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	_uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Shader_Material(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXANIMMESH::SHADOW));

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CPlayerBody::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Player"),
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

	m_pAnimMachine = CPlayerAnimMachine::Create();
	if (nullptr == m_pAnimMachine)
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerBody::Ready_BodyColors()
{
	_float3* pHairColor = new _float3[3];

	pHairColor[0] = _float3(0.4f, 0.4f, 0.4f);
	pHairColor[1] = _float3(1.f, 1.f, 0.f);
	pHairColor[2] = _float3(0.f, 0.f, 0.f);

	_float3* pUpperColor = new _float3[3];

	pUpperColor[0] = _float3(1.f, 1.f, 1.f);
	pUpperColor[1] = _float3(0.8f, 0.8f, 0.8f);
	pUpperColor[2] = _float3(0.2f, 0.2f, 0.2f);

	_float3* pLowerColor = new _float3[3];

	pLowerColor[0] = _float3(0.2f, 0.2f, 0.2f);
	pLowerColor[1] = _float3(1.f, 1.f, 1.f);
	pLowerColor[2] = _float3(0.2f, 0.2f, 0.2f);

	m_BodyColors.emplace(HAIR, pHairColor);
	m_BodyColors.emplace(UPPER, pUpperColor);
	m_BodyColors.emplace(LOWER, pLowerColor);

	return S_OK;
}

HRESULT CPlayerBody::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pPawnMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerBody::Bind_Color(_uint iPartIndex)
{
	_float3* pColors = m_BodyColors[static_cast<PART>(iPartIndex)];

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_R", &pColors[0], sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_G", &pColors[1], sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor_B", &pColors[2], sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CPlayerBody* CPlayerBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayerBody* pInstance = new CPlayerBody(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPlayerBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerBody::Clone(void* pArg)
{
	CPlayerBody* pInstance = new CPlayerBody(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPlayerBody"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerBody::Free()
{
	__super::Free();

	Safe_Release(m_pAnimMachine);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);

	for (auto& Pair : m_BodyColors)
		Safe_Delete_Array(Pair.second);
}
