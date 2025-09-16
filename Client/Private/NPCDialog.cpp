#include "ClientPch.h"
#include "NPCDialog.h"
#include "Button.h"

CNPCDialog::CNPCDialog(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CNPCDialog::CNPCDialog(const CNPCDialog& Prototype)
	: CUI_Panel { Prototype }
{
}

HRESULT CNPCDialog::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPCDialog::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CNPCDialog::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::NPC_DIALOG))
		__super::Children_Priority_Update(fTimeDelta);
}

void CNPCDialog::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::NPC_DIALOG))
		__super::Children_Update(fTimeDelta);
}

void CNPCDialog::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::NPC_DIALOG))
		__super::Children_Late_Update(fTimeDelta);
}

HRESULT CNPCDialog::Render()
{
	return S_OK;
}

HRESULT CNPCDialog::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC DialogDesc{};
	DialogDesc.fX = m_fX;
	DialogDesc.fY = m_fY;
	DialogDesc.fSizeX = 550.f;
	DialogDesc.fSizeY = 300.f;
	DialogDesc.fOffsetX = -300.f;
	DialogDesc.fOffsetY = 200.f;
	DialogDesc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	DialogDesc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Text_Dialog");
	DialogDesc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	DialogDesc.IsBlend = true;
	DialogDesc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &DialogDesc)))
		return E_FAIL;

	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 70.f;
	Button_Desc.fSizeY = 30.f;
	Button_Desc.fOffsetX = -500.f;
	Button_Desc.fOffsetY = 320.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Dialog_Button");
	Button_Desc.IsBlend = true;
	Button_Desc.fAlpha = 0.7f;

	Button_Desc.Callback = [this]() {
		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY) | ENUM_CLASS(GAMEPLAY_FLAG::DYEING);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;


	Button_Desc.fOffsetX = -100.f;
	Button_Desc.fOffsetY = 320.f;

	Button_Desc.Callback = [this]() {
		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

CNPCDialog* CNPCDialog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNPCDialog* pInstance = new CNPCDialog(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CNPCDialog"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNPCDialog::Clone(void* pArg)
{
	CNPCDialog* pInstance = new CNPCDialog(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CNPCDialog"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNPCDialog::Free()
{
	__super::Free();
}
