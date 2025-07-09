#include "PlayerStateBar.h"

CPlayerStateBar::CPlayerStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProgressBar { pDevice, pDeviceContext}
{
}

CPlayerStateBar::CPlayerStateBar(const CPlayerStateBar& Prototype)
	: CProgressBar{ Prototype }
{
}

HRESULT CPlayerStateBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerStateBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRatio = 1.f;
	m_fCurrentRatio = 1.f;
	m_fFillSpeed = 0.5f;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CPlayerStateBar::Update(_float fTimeDelta)
{
	if(m_fCurrentRatio > m_fRatio)
	{
		m_fCurrentRatio = m_fRatio;
	}
	else if(m_fCurrentRatio < m_fRatio)
	{
		m_fCurrentRatio += m_fFillSpeed * fTimeDelta;
		if (m_fCurrentRatio >= m_fRatio)
			m_fCurrentRatio = m_fRatio;
	}


	__super::Children_Update(fTimeDelta);
}

HRESULT CPlayerStateBar::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	if (FAILED(CGameObject::Add_Component(iTexturePrototypeLevelIndex, strTexturePrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerStateBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerStateBar::Ready_Children()
{


	return S_OK;
}

CPlayerStateBar* CPlayerStateBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayerStateBar* pInstance = new CPlayerStateBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPlayerStateBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerStateBar::Clone(void* pArg)
{
	CPlayerStateBar* pInstance = new CPlayerStateBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPlayerStateBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerStateBar::Free()
{
	__super::Free();
}
