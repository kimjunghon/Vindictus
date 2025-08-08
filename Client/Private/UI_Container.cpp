#include "ClientPch.h"
#include "UI_Container.h"
#include "UI_Panel.h"
#include "Mouse.h"

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
	if (m_iUIState & ENUM_CLASS(STATE_FLAG::GAMEPLAY))
		Bind_InputData();

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
	m_pGameInstance->UI_Input(&m_UI_Input);

	if (m_UI_Input.bOption)
	{
		if (m_iUIState & ~ENUM_CLASS(STATE_FLAG::GAMEPLAY))
			m_iUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
		else
			m_iUIState |= ENUM_CLASS(GAMEPLAY_FLAG::OPTION);
	}

	if (m_UI_Input.bMouse)
	{
		if (m_iUIState == ENUM_CLASS(STATE_FLAG::GAMEPLAY))
			m_iUIState |= ENUM_CLASS(GAMEPLAY_FLAG::MOUSE);
		else if(m_iUIState == (ENUM_CLASS(STATE_FLAG::GAMEPLAY) | ENUM_CLASS(GAMEPLAY_FLAG::MOUSE)))
			m_iUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
	}

	if (m_UI_Input.bInventory)
	{
		if (m_iUIState == ENUM_CLASS(STATE_FLAG::GAMEPLAY))
			m_iUIState |= ENUM_CLASS(GAMEPLAY_FLAG::INVENTORY);
		else if(m_iUIState & ENUM_CLASS(GAMEPLAY_FLAG::INVENTORY))
			m_iUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
	}
}

HRESULT CUI_Container::Add_UIObject(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, _uint iUI_LevelIndex, void* pArg)
{
	CUIObject* pUI = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pUI)
		return E_FAIL;

	m_UIObjects[iUI_LevelIndex].push_back(pUI);

	return S_OK;
}

HRESULT CUI_Container::Ready_Loading_UI()
{
	CUI_Panel::PANEL_DESC Panel_Desc = {};

	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0;
	Panel_Desc.fOffsetY = 0;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.StateDesc.iUIState = &m_iUIState;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"), ENUM_CLASS(UI_LEVEL::LOADING), &Panel_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Container::Ready_Logo_UI()
{
	CUI_Panel::PANEL_DESC Panel_Desc{};
	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0.f;
	Panel_Desc.fOffsetY = 0.f;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.StateDesc.iUIState = &m_iUIState;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LogoScreen"), ENUM_CLASS(UI_LEVEL::LOGO), &Panel_Desc)))
		return E_FAIL;

	CMouse::UI_MOUSE_DESC Mouse_Desc{};
	Mouse_Desc.fX = g_iWinSizeX >> 1;
	Mouse_Desc.fY = g_iWinSizeY >> 1;
	Mouse_Desc.fSizeX = 30.f;
	Mouse_Desc.fSizeY = 30.f;
	Mouse_Desc.fOffsetX = 10.f;
	Mouse_Desc.fOffsetY = 10.f;
	Mouse_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIFTH);
	Mouse_Desc.StateDesc.iUIState = &m_iUIState;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Mouse"), ENUM_CLASS(UI_LEVEL::LOGO), &Mouse_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Container::Ready_GamePlay_UI()
{
	CMouse::UI_MOUSE_DESC Mouse_Desc{};
	Mouse_Desc.fX = g_iWinSizeX >> 1;
	Mouse_Desc.fY = g_iWinSizeY >> 1;
	Mouse_Desc.fSizeX = 30.f;
	Mouse_Desc.fSizeY = 30.f;
	Mouse_Desc.fOffsetX = 0.f;
	Mouse_Desc.fOffsetY = 0.f;
	Mouse_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIFTH);
	Mouse_Desc.StateDesc.iUIState = &m_iUIState;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Mouse"), ENUM_CLASS(UI_LEVEL::GAMEPLAY), &Mouse_Desc)))
		return E_FAIL;

	CUI_Panel::PANEL_DESC Panel_Desc{};
	Panel_Desc.fX = g_iWinSizeX >> 1;
	Panel_Desc.fY = g_iWinSizeY >> 1;
	Panel_Desc.fSizeX = g_iWinSizeX;
	Panel_Desc.fSizeY = g_iWinSizeY;
	Panel_Desc.fOffsetX = 0.f;
	Panel_Desc.fOffsetY = 0.f;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	Panel_Desc.StateDesc.iUIState = &m_iUIState;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_HUD"), ENUM_CLASS(UI_LEVEL::GAMEPLAY), &Panel_Desc)))
		return E_FAIL;

	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Option"), ENUM_CLASS(UI_LEVEL::GAMEPLAY), &Panel_Desc)))
		return E_FAIL;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_OptionController"), ENUM_CLASS(UI_LEVEL::GAMEPLAY), &Panel_Desc)))
		return E_FAIL;

	if (FAILED(Add_UIObject(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Inventory"), ENUM_CLASS(UI_LEVEL::GAMEPLAY), &Panel_Desc)))
		return E_FAIL;

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
