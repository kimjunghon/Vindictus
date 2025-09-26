#include "ClientPch.h"
#include "DyeingPalette.h"
#include "Palette.h"
#include "Button.h"

CDyeingPalette::CDyeingPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CDyeingPalette::CDyeingPalette(const CDyeingPalette& Prototype)
	: CUI_Panel { Prototype }
{
}

HRESULT CDyeingPalette::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDyeingPalette::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CDyeingPalette::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CDyeingPalette::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CDyeingPalette::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CDyeingPalette::Render()
{
	return S_OK;
}

void CDyeingPalette::ClearPalette()
{
	m_pPalette->Clear();
}

void CDyeingPalette::Set_DyeingItem(pair<ITEM_TYPE, _uint>* pItemType)
{
	m_pPalette->Set_DyeingItem(pItemType);
}

HRESULT CDyeingPalette::Ready_Children()
{
	CUIObject::UIOBJECT_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 256.f;
	Children_Desc.fSizeY = 256.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = -55.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Palette"), &Children_Desc, reinterpret_cast<CUIObject**>(&m_pPalette))))
		return E_FAIL;

	CTextureUI::TEXTURE_UI_DESC PaletteBack{};
	PaletteBack.fX = m_fX;
	PaletteBack.fY = m_fY;
	PaletteBack.fSizeX = 320.f;
	PaletteBack.fSizeY = 450.f;
	PaletteBack.fOffsetX = 0.f;
	PaletteBack.fOffsetY = 0.f;
	PaletteBack.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	PaletteBack.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_DyeingBack");
	PaletteBack.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	PaletteBack.IsBlend = true;
	PaletteBack.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &PaletteBack)))
		return E_FAIL;

	PaletteBack.fSizeX = 72.f;
	PaletteBack.fSizeY = 72.f;
	PaletteBack.fOffsetX = -191.f;
	PaletteBack.fOffsetY = -120.f;
	PaletteBack.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	PaletteBack.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_DyeingColorBack");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &PaletteBack)))
		return E_FAIL;

	PaletteBack.fSizeX = 48.f;
	PaletteBack.fSizeY = 46.f;
	PaletteBack.fOffsetX = 100.f;
	PaletteBack.fOffsetY = 135.f;
	PaletteBack.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	PaletteBack.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_DyeingColorBack");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &PaletteBack)))
		return E_FAIL;

	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 50.f;
	Button_Desc.fSizeY = 20.f;
	Button_Desc.fOffsetX = -110.f;
	Button_Desc.fOffsetY = 135.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Dialog_Button");
	Button_Desc.IsBlend = true;
	Button_Desc.fAlpha = 0.7f;

	Button_Desc.Callback = [this]() {m_pPalette->Change_DyeingPart(0);	};

	Button_Desc.IsButtonText = true;
	Button_Desc.strButtonText = TEXT("ÆÄÃ÷ A");

	Button_Desc.vScale = _float2(0.5f, 0.5f);
	Button_Desc.vOffset = _float2(-16.f, -5.5f);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetX = -40.f;
	Button_Desc.Callback = [this]() {m_pPalette->Change_DyeingPart(1);	};

	Button_Desc.strButtonText = TEXT("ÆÄÃ÷ B");
	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetX = 30.f;
	Button_Desc.Callback = [this]() {m_pPalette->Change_DyeingPart(2);	};
	
	Button_Desc.strButtonText = TEXT("ÆÄÃ÷ C");

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	Button_Desc.fSizeX = 70.f;
	Button_Desc.fSizeY = 40.f;
	Button_Desc.fOffsetX = -110.f;
	Button_Desc.fOffsetY = 185.f;
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_OptionButton");
	Button_Desc.Callback = [this]() { m_pPalette->Dyeing();	};
	
	Button_Desc.strButtonText = TEXT("¿°»ö");
	Button_Desc.vScale = _float2(1.f, 1.f);
	Button_Desc.vOffset = _float2(-20.f, -8.5f);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;


	Button_Desc.fOffsetX = 90.f;
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_OptionButton");
	Button_Desc.Callback = [this]() {*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);	};

	Button_Desc.strButtonText = TEXT("µ¹¾Æ°¡±â");
	Button_Desc.vScale = _float2(0.6f, 0.6f);
	Button_Desc.vOffset = _float2(-20.f, -7.5f);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

CDyeingPalette* CDyeingPalette::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDyeingPalette* pInstance = new CDyeingPalette(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CDyeingPalette"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDyeingPalette::Clone(void* pArg)
{
	CDyeingPalette* pInstance = new CDyeingPalette(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CDyeingPalette"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDyeingPalette::Free()
{
	__super::Free();

	Safe_Release(m_pPalette);
}
