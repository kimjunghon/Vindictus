#include "ClientPch.h"
#include "SwordTrail.h"
#include "Pool_Instance.h"

CSwordTrail::CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CEffect{ pDevice, pDeviceContext }
{
}

CSwordTrail::CSwordTrail(const CSwordTrail& Prototype)
	: CEffect{ Prototype }
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSwordTrail::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	_vector vRotationQuaternion = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(-90.f));

	XMStoreFloat4x4(&m_RotateMatrix, XMMatrixRotationQuaternion(vRotationQuaternion));

	return S_OK;
}

void CSwordTrail::Priority_Update(_float fTimeDelta)
{
}

void CSwordTrail::Update(_float fTimeDelta)
{

}

void CSwordTrail::Late_Update(_float fTimeDelta)
{
	if (false == *m_IsSwing)
	{
		if (m_pVIBufferCom->IsFinished())
		{
			ReturnToPool();
		}
		else
		{
			m_pVIBufferCom->Update(fTimeDelta);
			m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
		}
		return;
	}

	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	for (size_t i = 0; i < 3; i++)
		SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&m_RotateMatrix), SocketMatrix), XMLoadFloat4x4(m_pParentMatrix)));
	
	TRAIL_NODE Trail = {};

	XMStoreFloat3(&Trail.vTrailLeft, XMVector3TransformCoord(m_vLeftPosition, XMLoadFloat4x4(&m_CombinedMatrix)));
	XMStoreFloat3(&Trail.vTrailRight, XMVector3TransformCoord(m_vRightPosition, XMLoadFloat4x4(&m_CombinedMatrix)));
	Trail.vLifeTime = _float2(0.f, m_fLifeTime);

	m_pVIBufferCom->Update_TrailBuffer(Trail, fTimeDelta);

	m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CSwordTrail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSwordTrail::Spawn(void* pArg)
{
	m_IsActive = true;

	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_pParentMatrix = pDesc->pParentMatrix;
	m_IsSwing = pDesc->IsSwing;

	m_vLeftPosition = XMLoadFloat3(&pDesc->vLeftPosition);
	m_vRightPosition = XMLoadFloat3(&pDesc->vRightPosition);
	m_fLifeTime = pDesc->fLifeTime;

	m_pVIBufferCom->Clear();

	return S_OK;
}

void CSwordTrail::ReturnToPool()
{
	m_IsActive = false;

	m_pSocketMatrix = nullptr;
	m_pParentMatrix = nullptr;
	m_IsSwing = nullptr;

	m_vLeftPosition = XMVectorZero();
	m_vRightPosition = XMVectorZero();
	m_fLifeTime = 0.f;

	m_pVIBufferCom->Clear();

	m_pPool_Instance->ReturnPool(TEXT("SwordTrail"), this);
}

void CSwordTrail::Clear()
{
	m_pVIBufferCom->Clear();
}

HRESULT CSwordTrail::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordTrail"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTrail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	return S_OK;
}

CSwordTrail* CSwordTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSwordTrail* pInstance = new CSwordTrail(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created CSwordTrail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSwordTrail::Clone(void* pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned CSwordTrail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
