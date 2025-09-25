#include "ClientPch.h"
#include "LoadingBar.h"
#include "Bar.h"
#include "LoadingPoint.h"

CLoadingBar::CLoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{pDevice, pDeviceContext}
{
}

CLoadingBar::CLoadingBar(const CLoadingBar& Prototype)
	: CUI_Panel{ Prototype }
	, m_eType { Prototype.m_eType }
	, m_fRatio { Prototype.m_fRatio }
	, m_fBarRatio { Prototype.m_fBarRatio }
	, m_fFillSpeed { Prototype.m_fFillSpeed }
	, m_fPointX { Prototype.m_fPointX }
{
}

HRESULT CLoadingBar::Initialize_Prototype()
{
	m_eType = PROGRESS_TYPE::LOADING;

	m_fRatio = 0.f;
	m_fBarRatio = 0.f;
	m_fFillSpeed = 0.f;
	m_fPointX = m_fSizeX * 0.5f - (m_fSizeX * m_fBarRatio);

	return S_OK;
}

HRESULT CLoadingBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_PROGRESSBAR>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_PROGRESSBAR& Event) {
		this->Event_ProgressBar(Event); });
	return S_OK;
}

void CLoadingBar::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CLoadingBar::Update(_float fTimeDelta)
{
	Update_BarRatio(fTimeDelta);
	Update_PointOffset(fTimeDelta);

	__super::Children_Update(fTimeDelta);
}

void CLoadingBar::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CLoadingBar::Render()
{
	return S_OK;
}

HRESULT CLoadingBar::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iDepth = m_iDepth;
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_LoadingBar_Back");
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_LoadingBar");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"), &Children_Desc, reinterpret_cast<CUIObject**>(&m_pBar))))
		return E_FAIL;

	Children_Desc.fSizeX = 30.f;
	Children_Desc.fSizeY = 30.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_LoadingBar_Point");

	if(FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingPoint"), &Children_Desc, reinterpret_cast<CUIObject**>(&m_pLoadingPoint))))
		return E_FAIL;

	return S_OK;
}

void CLoadingBar::Event_ProgressBar(const EVENT_PROGRESSBAR& Event)
{
	if (Event.eType == m_eType)
	{
		m_fRatio = Event.fRatio;

		if (m_IsLoadingEnd)
		{
			m_IsLoadingEnd = false;
			m_fBarRatio = 0.f;
		}
	}
}

void CLoadingBar::Update_BarRatio(_float fTimeDelta)
{
	m_fFillSpeed = (1.f - m_fBarRatio) * m_fRatio;
	if (m_fFillSpeed <= 0.03f)
		m_fFillSpeed = 0.03f;

	m_fBarRatio += m_fFillSpeed * fTimeDelta;

	m_pBar->Set_Ratio(m_fBarRatio);

	if (m_fBarRatio >= 1.f && m_fRatio >= 1.f && false == m_IsLoadingEnd)
	{
		EVENT_LOADING_COMPLETE Event;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::NONSTATIC), Event);
		m_fRatio = 0.f;
		m_IsLoadingEnd = true;
	}
}

void CLoadingBar::Update_PointOffset(_float fTimeDelta)
{
	m_fPointX = m_fSizeX * 0.5f - (m_fSizeX * m_fBarRatio);

	m_pLoadingPoint->Set_Offset(-m_fPointX, 0.f);
	m_pLoadingPoint->Set_Position(m_fX, m_fY);
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

	Safe_Release(m_pBar);
	Safe_Release(m_pLoadingPoint);
}
