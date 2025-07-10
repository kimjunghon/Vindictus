#include "StateBar_Lerp.h"

CStateBar_Lerp::CStateBar_Lerp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CStateBar{ pDevice, pDeviceContext }
{
}

CStateBar_Lerp::CStateBar_Lerp(const CStateBar_Lerp& Prototype)
	: CStateBar { Prototype }
{
}

HRESULT CStateBar_Lerp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStateBar_Lerp::Initialize(void* pArg)
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

void CStateBar_Lerp::Update(_float fTimeDelta)
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

	__super::Update(fTimeDelta);
}

CStateBar_Lerp* CStateBar_Lerp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStateBar_Lerp* pInstance = new CStateBar_Lerp(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CStateBar_Lerp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStateBar_Lerp::Clone(void* pArg)
{
	CStateBar_Lerp* pInstance = new CStateBar_Lerp(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CStateBar_Lerp"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateBar_Lerp::Free()
{
	__super::Free();
}