#include "ClientPch.h"
#include "OptionController.h"
#include "Button.h"

COptionController::COptionController(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{ pDevice, pDeviceContext }
{
}

COptionController::COptionController(const COptionController& Prototype)
	: CUI_Panel{ Prototype }
{
}

HRESULT COptionController::Initialize_Prototype()
{

	return S_OK;
}

HRESULT COptionController::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	Ready_Children();

	return S_OK;
}

void COptionController::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::CONTROLLER))
		__super::Children_Priority_Update(fTimeDelta);
}

void COptionController::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::CONTROLLER))
		__super::Children_Update(fTimeDelta);
}

void COptionController::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::CONTROLLER))
		__super::Children_Late_Update(fTimeDelta);
}


HRESULT COptionController::Render()
{
    return S_OK;
}

HRESULT COptionController::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Background_Desc{};
	Background_Desc.fX = m_fX;
	Background_Desc.fY = m_fY;
	Background_Desc.fSizeX = 600.f;
	Background_Desc.fSizeY = 512.f;
	Background_Desc.fOffsetX = 0.f;
	Background_Desc.fOffsetY = 0.f;
	Background_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Background_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Option_Background");
	Background_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Background_Desc)))
		return E_FAIL;

	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 300.f;
	Button_Desc.fSizeY = 150.f;
	Button_Desc.fOffsetX = 0.f;
	Button_Desc.fOffsetY = -150.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Keyboard");

	Button_Desc.Callback = [this]() {
		m_strChangeControllerTag = TEXT("Controller_KeyBoard");
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetY += 200.f;
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Mouse");

	Button_Desc.Callback = [this]() {
		m_strChangeControllerTag = TEXT("Controller_Mouse");
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fSizeX = 100.f;
	Button_Desc.fSizeY = 50.f;
	Button_Desc.fOffsetX += 150.f;
	Button_Desc.fOffsetY += 150.f;
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_OptionButton");
	Button_Desc.Callback = [this]() {
		m_pGameInstance->Change_Controller(ENUM_CLASS(CONTROLLER_CHANNEL::MAIN), m_strChangeControllerTag);
		m_strChangeControllerTag = {};

		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY) | ENUM_CLASS(GAMEPLAY_FLAG::OPTION);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

COptionController* COptionController::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	COptionController* pInstance = new COptionController(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : COptionController"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* COptionController::Clone(void* pArg)
{
	COptionController* pInstance = new COptionController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : COptionController"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COptionController::Free()
{
	__super::Free();
}
