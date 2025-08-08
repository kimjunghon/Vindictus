#include "ClientPch.h"
#include "StateBar.h"
#include "Bar.h"

CStateBar::CStateBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Slot{ pDevice, pDeviceContext }
{
}

CStateBar::CStateBar(const CStateBar& Prototype)
	: CUI_Slot{ Prototype }
	, m_fRatio{ Prototype.m_fRatio }
	, m_fBarRatio{ Prototype.m_fBarRatio }
	, m_fLerpBarRatio { Prototype.m_fLerpBarRatio }
	, m_fFillSpeed{ Prototype.m_fFillSpeed }
{
}

HRESULT CStateBar::Initialize_Prototype()
{
	if (FAILED(CUI_Slot::Initialize_Prototype(ENUM_CLASS(STATE_SLOT::END))))
		return E_FAIL;

	m_fRatio = 1.f;
	m_fBarRatio = 1.f;
	m_fLerpBarRatio = 1.f;
	m_fFillSpeed = 0.5f;

	return S_OK;
}

HRESULT CStateBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	STATE_BAR_DESC* pDesc = static_cast<STATE_BAR_DESC*>(pArg);
	m_eType = pDesc->eType;

	Ready_TypeDesc();

	if (FAILED(Ready_Children()))
		return E_FAIL;

	m_pBar = static_cast<CBar*>(m_Children[ENUM_CLASS(STATE_SLOT::BAR)]);
	Safe_AddRef(m_pBar);

	m_pLerpBar = static_cast<CBar*>(m_Children[ENUM_CLASS(STATE_SLOT::LERP_BAR)]);
	Safe_AddRef(m_pLerpBar);

	m_pGameInstance->Subscribe<EVENT_PROGRESSBAR>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_PROGRESSBAR& Event) {
		this->Event_ProgressBar(Event); });

	return S_OK;
}

void CStateBar::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CStateBar::Update(_float fTimeDelta)
{
	Update_BarRatio(fTimeDelta);
	Update_LerpBarRatio(fTimeDelta);

	__super::Children_Update(fTimeDelta);
}

void CStateBar::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CStateBar::Render()
{
	return S_OK;
}

void CStateBar::Ready_TypeDesc()
{
	switch (m_eType)
	{
	case PROGRESS_TYPE::HP:
		m_strType = TEXT("PlayerHpBar");
		m_fBackSizeX = 68.f;
		m_fBackSizeY = 8.f;
		break;

	case PROGRESS_TYPE::STAMINA:
		m_strType = TEXT("PlayerStaminaBar");
		m_fBackSizeX = 35.f;
		m_fBackSizeY = 7.f;
		break;

//	case PROGRESS_TYPE::SP:
//		break;

	}
}


HRESULT CStateBar::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_") + m_strType;
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(CUI_Slot::Add_Child(ENUM_CLASS(STATE_SLOT::BAR), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Lerp_") + m_strType;

	if (FAILED(CUI_Slot::Add_Child(ENUM_CLASS(STATE_SLOT::LERP_BAR), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"), &Children_Desc)))
		return E_FAIL;
	
	Children_Desc.fX += 4.f;
	Children_Desc.fSizeX += m_fBackSizeX;
	Children_Desc.fSizeY += m_fBackSizeY;
	Children_Desc.iDepth = m_iDepth;
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Back_") + m_strType;

	if(FAILED(CUI_Slot::Add_Child(ENUM_CLASS(STATE_SLOT::BACKGROUND), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

void CStateBar::Event_ProgressBar(const EVENT_PROGRESSBAR& Event)
{
	if (Event.eType == m_eType)
		m_fRatio = Event.fRatio;
}

void CStateBar::Update_BarRatio(_float fTimeDelta)
{
	if (m_fBarRatio > m_fRatio)
	{
		m_fBarRatio = m_fRatio;
	}
	else if (m_fBarRatio < m_fRatio)
	{
		m_fBarRatio += m_fFillSpeed * fTimeDelta;
		if (m_fBarRatio >= m_fRatio)
			m_fBarRatio = m_fRatio;
	}

	m_pBar->Set_Ratio(m_fBarRatio);
}

void CStateBar::Update_LerpBarRatio(_float fTimeDelta)
{
	if (m_fLerpBarRatio < m_fRatio)
	{
		m_fLerpBarRatio = m_fRatio;
	}
	else if (m_fLerpBarRatio > m_fRatio)
	{
		m_fLerpBarRatio -= m_fFillSpeed * fTimeDelta;
		if (m_fLerpBarRatio <= m_fRatio)
			m_fLerpBarRatio = m_fRatio;
	}

	m_pLerpBar->Set_Ratio(m_fLerpBarRatio);
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

	Safe_Release(m_pBar);
	Safe_Release(m_pLerpBar);
}