#include "ClientPch.h"
#include "Level_Logo.h"
#include "Level_Loading.h"

#include "UIObject.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Ready_Layer(TEXT("Layer"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Logo::Update(_float fTimeDleta)
{
	if (GetKeyState(VK_RETURN) & 0x8000)
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::GAMEPLAY);
		Event.bIsLoading = false;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}
}

HRESULT CLevel_Logo::Render()
{
	SetWindowText(g_hWnd, TEXT("로고레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer(const _wstring& strLayerTag)
{
	CUIObject::UIOBJECT_DESC UI_Desc{};
	UI_Desc.fX = g_iWinSizeX >> 1;
	UI_Desc.fY = g_iWinSizeY >> 1;
	UI_Desc.fSizeX = g_iWinSizeX;
	UI_Desc.fSizeY = g_iWinSizeY;
	UI_Desc.fOffsetX = 0;
	UI_Desc.fOffsetY = 0;
	UI_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_UIObject_LogoScreen"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_UI"), &UI_Desc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Logo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
}
