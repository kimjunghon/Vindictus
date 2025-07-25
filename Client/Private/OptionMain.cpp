#include "ClientPch.h"
#include "OptionMain.h"
#include "Button.h"

COptionMain::COptionMain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{pDevice, pDeviceContext }
{
}

COptionMain::COptionMain(const COptionMain& Prototype)
	: CUI_Panel{ Prototype }
	, m_byVisibleType{ Prototype.m_byVisibleType }
{
}

HRESULT COptionMain::Initialize_Prototype()
{
	m_byVisibleType = ENUM_CLASS(GAMEPLAY_UI::OPTION);

	return S_OK;
}

HRESULT COptionMain::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;

	Ready_Children();

	m_pGameInstance->Subscribe<EVENT_UI_CHANGE>(ENUM_CLASS(LEVEL::GAMEPLAY), [this](const EVENT_UI_CHANGE& Event) {
		this->Event_UI_Change(Event); });

	return S_OK;
}

void COptionMain::Priority_Update(_float fTimeDelta)
{
	if (m_bVisible)
		__super::Children_Priority_Update(fTimeDelta);
}

void COptionMain::Update(_float fTimeDelta)
{
	if (m_bVisible)
		__super::Children_Update(fTimeDelta);
}

void COptionMain::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
		__super::Children_Late_Update(fTimeDelta);
}


HRESULT COptionMain::Render()
{
	return S_OK;
}

HRESULT COptionMain::Ready_Children()
{
	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 150.f;
	Button_Desc.fSizeY = 70.f;
	Button_Desc.fOffsetX = 0.f;
	Button_Desc.fOffsetY = -100.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::GAMEPLAY);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_OptionButton");

	Button_Desc.Callback = [this]() {
		EVENT_UI_CHANGE Event;
		Event.byVisibleType = ENUM_CLASS(GAMEPLAY_UI::DEFAULT);
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::GAMEPLAY), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY += 120.f;

	Button_Desc.Callback = [this]() {
		EVENT_UI_CHANGE Event;
		Event.byVisibleType = ENUM_CLASS(GAMEPLAY_UI::DEFAULT) + ENUM_CLASS(GAMEPLAY_UI::CONTROLLER);
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::GAMEPLAY), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY += 120.f;
	Button_Desc.Callback = [this]() {
		//DestroyWindow(g_hWnd);
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::LOGO);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

void COptionMain::Event_UI_Change(const EVENT_UI_CHANGE& Event)
{
	if (Event.byVisibleType & m_byVisibleType)
		m_bVisible = true;
	else
		m_bVisible = false;
}

COptionMain* COptionMain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	COptionMain* pInstance = new COptionMain(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : COptionMain"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* COptionMain::Clone(void* pArg)
{
	COptionMain* pInstance = new COptionMain(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : COptionMain"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COptionMain::Free()
{
	__super::Free();
}
