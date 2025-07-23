#include "ClientPch.h"
#include "LoadingBar.h"
#include "Bar.h"

CLoadingBar::CLoadingBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Slot{pDevice, pDeviceContext}
{
}

CLoadingBar::CLoadingBar(const CLoadingBar& Prototype)
	: CUI_Slot{ Prototype }
	, m_eType { Prototype.m_eType }
	, m_fRatio { Prototype.m_fRatio }
	, m_fBarRatio { Prototype.m_fBarRatio }
	, m_fFillSpeed { Prototype.m_fFillSpeed }
	, m_fPointX { Prototype.m_fPointX }
{
}

HRESULT CLoadingBar::Initialize_Prototype()
{
	if (FAILED(CUI_Slot::Initialize_Prototype(ENUM_CLASS(LOADING_SLOT::END))))
		return E_FAIL;

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

	m_pBar = static_cast<CBar*>(m_Children[ENUM_CLASS(LOADING_SLOT::BAR)]);
	Safe_AddRef(m_pBar);

	m_pGameInstance->Subscribe<EVENT_PROGRESSBAR>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_PROGRESSBAR& Event) {
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

	if (FAILED(CUI_Slot::Add_Child(ENUM_CLASS(LOADING_SLOT::BACKGROUND), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_LoadingBar");

	if (FAILED(CUI_Slot::Add_Child(ENUM_CLASS(LOADING_SLOT::BAR), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeX = 30.f;
	Children_Desc.fSizeY = 30.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_LoadingBar_Point");

	if(FAILED(CUI_Slot::Add_Child(ENUM_CLASS(LOADING_SLOT::POINT), ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingPoint"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

void CLoadingBar::Event_ProgressBar(const EVENT_PROGRESSBAR& Event)
{
	if (Event.eType == m_eType)
		m_fRatio = Event.fRatio;
}

void CLoadingBar::Update_BarRatio(_float fTimeDelta)
{
	m_fFillSpeed = (1.f - m_fBarRatio) * m_fRatio;
	if (m_fFillSpeed <= 0.03f)
		m_fFillSpeed = 0.03f;

	m_fBarRatio += m_fFillSpeed * fTimeDelta;

	m_pBar->Set_Ratio(m_fBarRatio);

	if (m_fBarRatio >= 1.f)
	{
		EVENT_LOADING_COMPLETE Event;
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::LOADING), Event);
	}
}

void CLoadingBar::Update_PointOffset(_float fTimeDelta)
{
	m_fPointX = m_fSizeX * 0.5f - (m_fSizeX * m_fBarRatio);

	m_Children[ENUM_CLASS(LOADING_SLOT::POINT)]->Set_Offset(-m_fPointX, 0.f);
	m_Children[ENUM_CLASS(LOADING_SLOT::POINT)]->Set_Position(m_fX, m_fY);
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
}
