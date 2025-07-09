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
	m_fFillSpeed = 0.f;
	
	m_eType = PROGRESS_TYPE::LOADING;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CLoadingBar::Update(_float fTimeDelta)
{
	m_fFillSpeed = (1.f - m_fCurrentRatio) * m_fRatio;
	if (m_fFillSpeed <= 0.03f)
		m_fFillSpeed = 0.03f;
	
	m_fCurrentRatio += m_fFillSpeed * fTimeDelta;

	m_fPointX = m_fStartX + (m_fSizeX * m_fCurrentRatio);

	__super::Update_ChildPosition(m_fPointX, m_fY);

	if (m_fCurrentRatio >= 1.f)
	{
		EVENT_LOADING_COMPLETE Event;
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::LOADING), Event);
	}

	__super::Children_Update(fTimeDelta);
}

HRESULT CLoadingBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingBar::Ready_Children()
{
	UIOBJECT_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 100.f;
	Children_Desc.fSizeY = 100.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = m_fOffsetY;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIFTH);

	if (FAILED(CUIObject::Add_StaticTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingPoint"), &Children_Desc)))
		return E_FAIL;

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
