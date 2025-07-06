#include "ButtonObject.h"

CButtonObject::CButtonObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CButtonObject::CButtonObject(const CUIObject& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CButtonObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CButtonObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	BUTTONOBJECT_DESC* pDesc = static_cast<BUTTONOBJECT_DESC*>(pArg);
	m_Callback = pDesc->Callback;

	return S_OK;
}

void CButtonObject::Priority_Update(_float fTimeDelta)
{
}

void CButtonObject::Update(_float fTimeDelta)
{
}

void CButtonObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CButtonObject::Render()
{
	return S_OK;
}

HRESULT CButtonObject::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	return S_OK;
}

void CButtonObject::Free()
{
	__super::Free();
}
