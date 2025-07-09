#include "PlayerStateBar_Lerp.h"

CPlayerStateBar_Lerp::CPlayerStateBar_Lerp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CStateBar{ pDevice, pDeviceContext }
{
}

CPlayerStateBar_Lerp::CPlayerStateBar_Lerp(const CPlayerStateBar_Lerp& Prototype)
	: CStateBar { Prototype }
{
}

HRESULT CPlayerStateBar_Lerp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayerStateBar_Lerp::Initialize(void* pArg)
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

void CPlayerStateBar_Lerp::Update(_float fTimeDelta)
{
	if (m_fCurrentRatio < m_fRatio)
	{
		m_fCurrentRatio = m_fRatio;
	}
	else if (m_fCurrentRatio > m_fRatio)
	{
		m_fCurrentRatio -= m_fFillSpeed * fTimeDelta;
		if (m_fCurrentRatio <= m_fRatio)
			m_fCurrentRatio = m_fRatio;
	}
}

CPlayerStateBar_Lerp* CPlayerStateBar_Lerp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPlayerStateBar_Lerp* pInstance = new CPlayerStateBar_Lerp(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CPlayerStateBar_Lerp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerStateBar_Lerp::Clone(void* pArg)
{
	CPlayerStateBar_Lerp* pInstance = new CPlayerStateBar_Lerp(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CPlayerStateBar_Lerp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerStateBar_Lerp::Free()
{
	__super::Free();
}