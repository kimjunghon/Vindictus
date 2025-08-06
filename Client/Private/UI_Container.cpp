#include "ClientPch.h"
#include "UI_Container.h"
#include "UI_Panel.h"

CUI_Container::CUI_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
}

CUI_Container::CUI_Container(const CUI_Container& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CUI_Container::Initialize_Prototype()
{


	return S_OK;
}

HRESULT CUI_Container::Initialize(void* pArg)
{

	if (FAILED(Ready_Loading_UI()))
		return E_FAIL;

	if (FAILED(Ready_Logo_UI()))
		return E_FAIL;

	if (FAILED(Ready_GamePlay_UI()))
		return E_FAIL;

	m_iArrayState = ENUM_CLASS(UI_LEVEL::LOADING);
	m_iUIState = ENUM_CLASS(STATE_FLAG::LOADING) | ENUM_CLASS(LOADING_FLAG::LOGO);

	m_pGameInstance->Subscribe<EVENT_UI_LEVEL_CHANGE>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_UI_LEVEL_CHANGE& Event) {
		this->Event_LevelChange(Event); });

	return S_OK;
}

void CUI_Container::Priority_Update(_float fTimeDelta)
{
	for (auto& pUI : m_UIObjects[m_iArrayState])
		pUI->Priority_Update(fTimeDelta);
}

void CUI_Container::Update(_float fTimeDelta)
{
	for (auto& pUI : m_UIObjects[m_iArrayState])
		pUI->Update(fTimeDelta);
}

void CUI_Container::Late_Update(_float fTimeDelta)
{
	for (auto& pUI : m_UIObjects[m_iArrayState])
		pUI->Late_Update(fTimeDelta);
}

HRESULT CUI_Container::Render()
{
	return S_OK;
}

void CUI_Container::Bind_InputData()
{
}

HRESULT CUI_Container::Ready_Loading_UI()
{
	CUI_Panel::PANEL_DESC Panel_Desc{};

	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0;
	Panel_Desc.fOffsetY = 0;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.iUIState = &m_iUIState;

	CUIObject* pUI = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"), &Panel_Desc));
	if (nullptr == pUI)
		return E_FAIL;

	m_UIObjects[ENUM_CLASS(UI_LEVEL::LOADING)].push_back(pUI);

	return S_OK;
}

HRESULT CUI_Container::Ready_Logo_UI()
{
	CUI_Panel::PANEL_DESC Panel_Desc{};
	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0;
	Panel_Desc.fOffsetY = 0;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.iUIState = &m_iUIState;

	CUIObject* pUI = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LogoScreen"), &Panel_Desc));
	if (nullptr == pUI)
		return E_FAIL;

	m_UIObjects[ENUM_CLASS(UI_LEVEL::LOGO)].push_back(pUI);

	return S_OK;
}

HRESULT CUI_Container::Ready_GamePlay_UI()
{
	CUI_Panel::PANEL_DESC Panel_Desc{};
	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0.f;
	Panel_Desc.fOffsetY = 0.f;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.iUIState = &m_iUIState;

	CUIObject* pHUD = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_HUD"), &Panel_Desc));
	if (nullptr == pHUD)
		return E_FAIL;

	m_UIObjects[ENUM_CLASS(UI_LEVEL::GAMEPLAY)].push_back(pHUD);

	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	CUIObject* pOption = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Option"), &Panel_Desc));
	if (nullptr == pOption)
		return E_FAIL;

	m_UIObjects[ENUM_CLASS(UI_LEVEL::GAMEPLAY)].push_back(pOption);

	CUIObject* pOptionController = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_OptionController"), &Panel_Desc));
	if (nullptr == pOptionController)
		return E_FAIL;

	m_UIObjects[ENUM_CLASS(UI_LEVEL::GAMEPLAY)].push_back(pOptionController);

	return S_OK;
}

void CUI_Container::Event_LevelChange(const EVENT_UI_LEVEL_CHANGE& Event)
{
	if (Event.bIsLoading)
	{
		switch (Event.iChange_Level)
		{
		case ENUM_CLASS(LEVEL::LOGO):
			m_iArrayState = ENUM_CLASS(UI_LEVEL::LOGO);
			m_iUIState = ENUM_CLASS(STATE_FLAG::LOGO);
			break;
		default:
			m_iArrayState = ENUM_CLASS(UI_LEVEL::GAMEPLAY);
			m_iUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
			break;
		}
	}
	else
	{
		m_iArrayState = ENUM_CLASS(UI_LEVEL::LOADING);

		_uint iLoadingFlag = {};

		switch (Event.iChange_Level)
		{
		case ENUM_CLASS(LEVEL::LOGO):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::LOGO);
			break;
		case ENUM_CLASS(LEVEL::TOWN):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::TOWN);
			break;
		case ENUM_CLASS(LEVEL::FIELD):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::FIELD);
			break;
		case ENUM_CLASS(LEVEL::QUEEN):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::QUEEN);
			break;
		case ENUM_CLASS(LEVEL::GLASGAVELEN):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::GLASGAVELEN);
			break;
			//юс╫ц
		case ENUM_CLASS(LEVEL::GAMEPLAY):
			iLoadingFlag = ENUM_CLASS(LOADING_FLAG::TOWN);
			break;
		}

		m_iUIState = ENUM_CLASS(STATE_FLAG::LOADING) | iLoadingFlag;
	}
}

CUI_Container* CUI_Container::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Container* pInstance = new CUI_Container(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CUI_Container"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUI_Container::Clone(void* pArg)
{
	CUI_Container* pInstance = new CUI_Container(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CUI_Container"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Container::Free()
{
	__super::Free();

	for (_uint i = 0; i < ENUM_CLASS(UI_LEVEL::END); i++)
	{
		for (auto& pUI : m_UIObjects[i])
			Safe_Release(pUI);
		m_UIObjects[i].clear();
	}
}
