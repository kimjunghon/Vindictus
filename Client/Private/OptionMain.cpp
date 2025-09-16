#include "ClientPch.h"
#include "OptionMain.h"
#include "Button.h"

COptionMain::COptionMain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{pDevice, pDeviceContext }
{
}

COptionMain::COptionMain(const COptionMain& Prototype)
	: CUI_Panel{ Prototype }
{
}

HRESULT COptionMain::Initialize_Prototype()
{

	return S_OK;
}

HRESULT COptionMain::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Children();

	return S_OK;
}

void COptionMain::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::OPTION))
		__super::Children_Priority_Update(fTimeDelta);
}

void COptionMain::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::OPTION))
		__super::Children_Update(fTimeDelta);
}

void COptionMain::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::OPTION))
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
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_OptionButton");
	Button_Desc.IsBlend = false;
	Button_Desc.fAlpha = 1.f;

	Button_Desc.Callback = [this]() {
		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY += 120.f;

	Button_Desc.Callback = [this]() {
		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY) | ENUM_CLASS(GAMEPLAY_FLAG::CONTROLLER);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY += 120.f;
	Button_Desc.Callback = [this]() {
		//DestroyWindow(g_hWnd);
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::LOGO);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
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
