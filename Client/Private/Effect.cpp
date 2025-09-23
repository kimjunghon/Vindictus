#include "ClientPch.h"
#include "Effect.h"
#include "Pool_Instance.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
	, m_pPool_Instance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPool_Instance);
}

CEffect::CEffect(const CEffect& Prototype)
	: CGameObject{ Prototype }
	, m_pPool_Instance { Prototype.m_pPool_Instance }
{
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CEffect::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (nullptr == pArg)
		return S_OK;

	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);
	m_iPassIndex = pDesc->iPassIndex;
	m_strEffectName = pDesc->strEffectName;
	m_iCurrentLevel = pDesc->iLevel;

	return S_OK;
}

void CEffect::Priority_Update(_float fTimeDelta)
{
}

void CEffect::Update(_float fTimeDelta)
{
	int a = 10;
}

void CEffect::Late_Update(_float fTimeDelta)
{
}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::ReturnToPool()
{
	m_IsActive = false;

	m_pPool_Instance->ReturnPool(m_strEffectName, this);
}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pPool_Instance);
}
