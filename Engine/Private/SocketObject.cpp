#include "EnginePch.h"
#include "SocketObject.h"

CSocketObject::CSocketObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CPawnObject { pDevice, pDeviceContext }
{
}

CSocketObject::CSocketObject(const CSocketObject& Prototype)
	: CPawnObject { Prototype }
{
}

HRESULT CSocketObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSocketObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	SOCKETOBJECT_DESC* pDesc = static_cast<SOCKETOBJECT_DESC*>(pArg);
	m_pSocketMatrix = pDesc->pSocketMatrix;

	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixIdentity());

	return S_OK;
}

void CSocketObject::Priority_Update(_float fTimeDelta)
{
}

void CSocketObject::Update(_float fTimeDelta)
{
}

void CSocketObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CSocketObject::Render()
{
	return S_OK;
}

void CSocketObject::Free()
{
	__super::Free();

}
