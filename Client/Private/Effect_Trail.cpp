#include "ClientPch.h"
#include "Effect_Trail.h"
#include "Pool_Instance.h"

CEffect_Trail::CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CEffect{ pDevice, pDeviceContext }
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& Prototype)
	: CEffect{ Prototype }
	, m_eType  { Prototype.m_eType }
	, m_RotateMatrix { Prototype.m_RotateMatrix }
	, m_vColor { Prototype.m_vColor }
{
}

HRESULT CEffect_Trail::Initialize_Prototype(TRAIL_TYPE eType, _fmatrix RotateMatrix, _float3 vColor)
{
	m_eType = eType;
	
	m_vColor = vColor;

	XMStoreFloat4x4(&m_RotateMatrix, RotateMatrix);

	return S_OK;
}

HRESULT CEffect_Trail::Initialize(void* pArg)
{
	if(FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;


	return S_OK;
}

void CEffect_Trail::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Trail::Update(_float fTimeDelta)
{

}

void CEffect_Trail::Late_Update(_float fTimeDelta)
{
	if(m_IsStop)
	{
		if (m_pVIBufferCom->IsFinished())
		{
			ReturnToPool();
		}
		else
		{

			if (m_eType == TRAIL_TYPE::STATIC)
				m_pVIBufferCom->Update(fTimeDelta);
			else
				m_pVIBufferCom->Update_Billboard(fTimeDelta);

			if(m_IsEmissive)
				m_pGameInstance->Add_RenderGroup(RENDERGROUP::EMISSIVE, this);
			else
				m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
		}
		return;
	}
	if (false == *m_IsSwing && false == m_IsStop)
	{
		m_IsStop = true;
	}

	if(m_pSocketMatrix)
	{
		_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

		for (size_t i = 0; i < 3; i++)
			SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);

		XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&m_RotateMatrix), SocketMatrix), XMLoadFloat4x4(m_pParentMatrix)));
	}
	else
		XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_RotateMatrix), XMLoadFloat4x4(m_pParentMatrix)));
	
	TRAIL_NODE Trail = {};

	XMStoreFloat3(&Trail.vTrailLeft, XMVector3TransformCoord(m_vLeftPosition, XMLoadFloat4x4(&m_CombinedMatrix)));
	XMStoreFloat3(&Trail.vTrailRight, XMVector3TransformCoord(m_vRightPosition, XMLoadFloat4x4(&m_CombinedMatrix)));
	Trail.vLifeTime = _float2(0.f, m_fLifeTime);
	
	m_fCurrentTime += fTimeDelta;

	if(m_fCurrentTime >= m_fNodeUpdateTime)
	{
		m_fCurrentTime = 0.f;

		if (m_eType == TRAIL_TYPE::STATIC)
			m_pVIBufferCom->Add_TrailBuffer(Trail, fTimeDelta);
		else
			m_pVIBufferCom->Add_TrailBuffer_Billboard(Trail, fTimeDelta);
	}

	if (m_IsEmissive)
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::EMISSIVE, this);
	else
		m_pGameInstance->Add_RenderGroup(RENDERGROUP::BLEND, this);
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_IsEmissive)
		m_pShaderCom->Begin(ENUM_CLASS(SHADER_TRAIL::EMISSIVE));
	else
		m_pShaderCom->Begin(ENUM_CLASS(SHADER_TRAIL::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CEffect_Trail::Spawn(void* pArg)
{
	m_IsActive = true;

	TRAIL_DESC* pDesc = static_cast<TRAIL_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;
	m_pParentMatrix = pDesc->pParentMatrix;
	m_IsSwing = pDesc->IsSwing;
	m_IsEmissive = pDesc->IsEmissive;

	m_vLeftPosition = XMLoadFloat3(&pDesc->vLeftPosition);
	m_vRightPosition = XMLoadFloat3(&pDesc->vRightPosition);
	m_fLifeTime = pDesc->fLifeTime;
	m_fNodeUpdateTime = pDesc->fNodeUpdateTime;

//	m_pVIBufferCom->Clear();

	return S_OK;
}

void CEffect_Trail::ReturnToPool()
{
	m_IsActive = false;

	m_pSocketMatrix = nullptr;
	m_pParentMatrix = nullptr;
	m_IsSwing = nullptr;
	m_IsStop = false;

	m_vLeftPosition = XMVectorZero();
	m_vRightPosition = XMVectorZero();
	m_fLifeTime = 0.f;
	m_fNodeUpdateTime = 0.f;
	m_pVIBufferCom->Clear();

	m_pPool_Instance->ReturnPool(m_strEffectName, this);
}

void CEffect_Trail::Clear()
{
	m_pVIBufferCom->Clear();
}

HRESULT CEffect_Trail::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Trail"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(m_iCurrentLevel, TEXT("Prototype_Component_Texture_") + m_strEffectName,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxTrail"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSourceColor", &m_vColor, sizeof(_float3))))
		return E_FAIL;


	return S_OK;
}

CEffect_Trail* CEffect_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TRAIL_TYPE eType, _fmatrix RotateMatrix, _float3 vColor)
{
	CEffect_Trail* pInstance = new CEffect_Trail(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, RotateMatrix, vColor)))
	{
		MSG_BOX(TEXT("Failed Created CEffect_Trail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffect_Trail::Clone(void* pArg)
{
	CEffect_Trail* pInstance = new CEffect_Trail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned CEffect_Trail"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Trail::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
