#include "ClientPch.h"
#include "Body.h"

CBody::CBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
{
}

CBody::CBody(const CBody& Prototype)
	: CPawnObject { Prototype }
{
}

HRESULT CBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BODY_DESC* pDesc = static_cast<BODY_DESC*>(pArg);
	m_pStateFlag = pDesc->pStateFlag;

	return S_OK;
}

void CBody::Priority_Update(_float fTimeDelta)
{
}

void CBody::Update(_float fTimeDelta)
{
}

void CBody::Late_Update(_float fTimeDelta)
{
}

HRESULT CBody::Render()
{
	return S_OK;
}

void CBody::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
