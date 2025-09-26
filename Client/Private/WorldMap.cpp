#include "ClientPch.h"
#include "WorldMap.h"
#include "Button.h"

CWorldMap::CWorldMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CWorldMap::CWorldMap(const CWorldMap& Prototype)
	: CUI_Panel { Prototype }
{
}

HRESULT CWorldMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldMap::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CWorldMap::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::WORLD_MAP))
		__super::Children_Priority_Update(fTimeDelta);
}

void CWorldMap::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::WORLD_MAP))
		__super::Children_Update(fTimeDelta);
}

void CWorldMap::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::WORLD_MAP))
		__super::Children_Late_Update(fTimeDelta);
}

HRESULT CWorldMap::Render()
{
	return S_OK;
}

HRESULT CWorldMap::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC DialogDesc{};
	DialogDesc.fX = m_fX;
	DialogDesc.fY = m_fY;
	DialogDesc.fSizeX = 1024.f;
	DialogDesc.fSizeY = 512.f;
	DialogDesc.fOffsetX = 0.f;
	DialogDesc.fOffsetY = 0.f;
	DialogDesc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	DialogDesc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_WorldMap");
	DialogDesc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	DialogDesc.IsBlend = false;
	DialogDesc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &DialogDesc)))
		return E_FAIL;

	CButton::BUTTON_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 128.f;
	Button_Desc.fSizeY = 128.f;
	Button_Desc.fOffsetX = 150.f;
	Button_Desc.fOffsetY = 150.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Button_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_QueenMap");
	Button_Desc.IsBlend = false;
	Button_Desc.fAlpha = 1.f;
	Button_Desc.Callback = [this]() {
		*m_pUIState = ENUM_CLASS(STATE_FLAG::GAMEPLAY);
		EVENT_LEVEL_CHANGE Event = {};
		Event.iChange_Level = ENUM_CLASS(LEVEL::QUEEN);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
		};

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"), &Button_Desc)))
		return E_FAIL;

	return S_OK;
}

CWorldMap* CWorldMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWorldMap* pInstance = new CWorldMap(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CWorldMap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CWorldMap::Clone(void* pArg)
{
	CWorldMap* pInstance = new CWorldMap(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CWorldMap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWorldMap::Free()
{
	__super::Free();
}
