#include "ClientPch.h"
#include "LogoScreen.h"
#include "Button.h"

CLogoScreen::CLogoScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{ pDevice, pDeviceContext }
{
}

CLogoScreen::CLogoScreen(const CLogoScreen& Prototype)
	: CUI_Panel{ Prototype }
{
}

HRESULT CLogoScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoScreen::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	if (FAILED(Ready_Button()))
		return E_FAIL;

	return S_OK;
}

void CLogoScreen::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CLogoScreen::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CLogoScreen::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CLogoScreen::Render()
{

	return S_OK;
}

HRESULT CLogoScreen::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_SkyBox");
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeX = 150.f;
	Children_Desc.fSizeY = 150.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = -150.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_Logo_WaterMark");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeX = 300.f;
	Children_Desc.fSizeY = 300.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_Logo_WaterMark_Back");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoScreen::Ready_Button()
{
	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 150.f;
	Button_Desc.fSizeY = 70.f;
	Button_Desc.fOffsetX = 0.f;
	Button_Desc.fOffsetY = 100.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_Logo_Button");

	Button_Desc.Callback = [&]() {
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::GAMEPLAY);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY = 200.f;
	Button_Desc.Callback = []() {
			DestroyWindow(g_hWnd);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

CLogoScreen* CLogoScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLogoScreen* pInstance = new CLogoScreen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogoScreen::Clone(void* pArg)
{
	CLogoScreen* pInstance = new CLogoScreen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoScreen::Free()
{
	__super::Free();
}
