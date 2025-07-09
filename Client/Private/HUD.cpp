#include "HUD.h"
#include "StateBar.h"

CHUD::CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CHUD::CHUD(const CHUD& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CHUD::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHUD::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	Ready_Children();

	return S_OK;
}

void CHUD::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CHUD::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CHUD::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CHUD::Render()
{
	return S_OK;
}

HRESULT CHUD::Ready_Children()
{
	if (FAILED(Ready_PlayerHPBar()))
		return E_FAIL;

	if (FAILED(Ready_PlayerStaminaBar()))
		return E_FAIL;


	return S_OK;
}

HRESULT CHUD::Ready_PlayerHPBar()
{
	CStateBar::PROGRESS_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 612.f;
	Children_Desc.fSizeY = 17.f;
	Children_Desc.fOffsetX = 200.f;
	Children_Desc.fOffsetY = -300.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.eType = PROGRESS_TYPE::HP;

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_PlayerStateBar"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHPBar"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_PlayerStateBar_Lerp"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHPBar_Lerp"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fX += 4.f;
	Children_Desc.fSizeX += 68.f;
	Children_Desc.fSizeY += 8.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHPBar_Back"), static_cast<UIOBJECT_DESC*>(&Children_Desc))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHUD::Ready_PlayerStaminaBar()
{
	CStateBar::PROGRESS_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 612.f;
	Children_Desc.fSizeY = 12.f;
	Children_Desc.fOffsetX = 200.f;
	Children_Desc.fOffsetY = -270.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);
	Children_Desc.eType = PROGRESS_TYPE::STAMINA;

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_PlayerStateBar"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_PlayerStateBar_Lerp"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar_Lerp"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fX += 4.f;
	Children_Desc.fSizeX += 35.f;
	Children_Desc.fSizeY += 7.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar_Back"), static_cast<UIOBJECT_DESC*>(&Children_Desc))))
		return E_FAIL;

	return S_OK;
}

CHUD* CHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHUD* pInstance = new CHUD(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CHUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHUD::Clone(void* pArg)
{
	CHUD* pInstance = new CHUD(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CHUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHUD::Free()
{
	__super::Free();
}