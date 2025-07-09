#include "StateBar.h"

CStateBar::CStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProgressBar { pDevice, pDeviceContext }
{
}

CStateBar::CStateBar(const CStateBar& Prototype)
	: CProgressBar { Prototype }
{
}

HRESULT CStateBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	PROGRESS_DESC* pDesc = static_cast<PROGRESS_DESC*>(pArg);
	m_eType = pDesc->eType;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CStateBar::Update(_float fTimeDelta)
{
}

HRESULT CStateBar::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	if (FAILED(CGameObject::Add_Component(iTexturePrototypeLevelIndex, strTexturePrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

void CStateBar::Free()
{
	__super::Free();
}

