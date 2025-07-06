#include "LoadingBar.h"
#include "GameInstance.h"

CLoadingBar::CLoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CProgressBar {pDevice, pDeviceContext}
{
}

CLoadingBar::CLoadingBar(const CLoadingBar& Prototype)
	: CProgressBar { Prototype }
{
}

HRESULT CLoadingBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_fRatio = 0.f;
	m_fCurrentRatio = 0.f;
	m_fFillSpeed = 0.5f;

	return S_OK;
}

void CLoadingBar::Priority_Update(_float fTimeDelta)
{
}

void CLoadingBar::Update(_float fTimeDelta)
{
	if (m_fCurrentRatio < m_fRatio)
	{
		m_fCurrentRatio += m_fFillSpeed * fTimeDelta;
		if (m_fCurrentRatio > m_fRatio)
		{
			m_fCurrentRatio = m_fRatio;
			EVENT_LOADING_COMPLETE Event;
			m_pGameInstance->Publish(ENUM_CLASS(LEVEL::LOADING), Event);
		}
	}
}

void CLoadingBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CLoadingBar::Render()
{
	CProgressBar::Render();

	return S_OK;
}

CLoadingBar* CLoadingBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLoadingBar* pInstance = new CLoadingBar(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CLoadingBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingBar::Clone(void* pArg)
{
	CLoadingBar* pInstance = new CLoadingBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CLoadingBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingBar::Free()
{
	__super::Free();
}
