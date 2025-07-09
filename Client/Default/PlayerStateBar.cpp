#include "PlayerStateBar.h"

CPlayerStateBar::CPlayerStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CStateBar{ pDevice, pDeviceContext}
{
}

CPlayerStateBar::CPlayerStateBar(const CPlayerStateBar& Prototype)
	: CStateBar{ Prototype }
{
}

HRESULT CPlayerStateBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerStateBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRatio = 1.f;
	m_fCurrentRatio = 1.f;
	m_fFillSpeed = 0.5f;

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
