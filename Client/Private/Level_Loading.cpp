#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
	if (m_pLoader->isFinished() &&
		GetKeyState(VK_SPACE) & 0x8000)
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(m_eNextLevelID);
		Event.bIsLoading = true;

		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
	}
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Show_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_GameObjects()
{
	CUIObject::UIOBJECT_DESC UI_Desc{};
	UI_Desc.fX = g_iWinSizeX >> 1;
	UI_Desc.fY = g_iWinSizeY >> 1;
	UI_Desc.fSizeX = g_iWinSizeX;
	UI_Desc.fSizeY = g_iWinSizeY;
	UI_Desc.fOffsetX = 0;
	UI_Desc.fOffsetY = 0;
	UI_Desc.iHeight = 2;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_UI"), &UI_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_LoadingThread()
{
	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, m_eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

CLevel_Loading* CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID)
{
	CLevel_Loading* pInstance = new CLevel_Loading(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed Created : CLevel_Loading"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
