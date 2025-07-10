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

	m_fRatio = 1.f;
	m_fCurrentRatio = 1.f;
	m_fFillSpeed = 0.5f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CStateBar::Update(_float fTimeDelta)
{
	if (m_fCurrentRatio > m_fRatio)
	{
		m_fCurrentRatio = m_fRatio;
	}
	else if (m_fCurrentRatio < m_fRatio)
	{
		m_fCurrentRatio += m_fFillSpeed * fTimeDelta;
		if (m_fCurrentRatio >= m_fRatio)
			m_fCurrentRatio = m_fRatio;
	}

	__super::Update(fTimeDelta);
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

CStateBar* CStateBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStateBar* pInstance = new CStateBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CStateBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStateBar::Clone(void* pArg)
{
	CStateBar* pInstance = new CStateBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CStateBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateBar::Free()
{
	__super::Free();
}