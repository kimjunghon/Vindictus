#include "Level_Loading.h"
#include "Loader.h"
#include "GameInstance.h"
#include "LoadingScreen.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel { pDevice, pDeviceContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	m_pGameInstance->Subscribe<EVENT_LOADING_COMPLETE>(ENUM_CLASS(LEVEL::LOADING), [this](const EVENT_LOADING_COMPLETE& Event) {
		this->Event_LoadingComplete(Event); });

	if (FAILED(Ready_GameObjects()))
		return E_FAIL;

	if (FAILED(Ready_LoadingThread()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Loading::Update(_float fTimeDelta)
{
}

HRESULT CLevel_Loading::Render()
{
	m_pLoader->Show_LoadingText();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_GameObjects()
{
	CLoadingScreen::LOADING_DESC Loading_Desc{};

	Loading_Desc.fX = g_iWinSizeX >> 1;
	Loading_Desc.fY = g_iWinSizeY >> 1;
	Loading_Desc.fSizeX = g_iWinSizeX;
	Loading_Desc.fSizeY = g_iWinSizeY;
	Loading_Desc.fOffsetX = 0;
	Loading_Desc.fOffsetY = 0;
	Loading_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Loading_Desc.iLoadingLevelID = ENUM_CLASS(m_eNextLevelID);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"),
		ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_UI"), &Loading_Desc)))
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

void CLevel_Loading::Event_LoadingComplete(const EVENT_LOADING_COMPLETE& Event)
{
	EVENT_LEVEL_CHANGE Event_LevelChange;
	Event_LevelChange.iChange_Level = ENUM_CLASS(m_eNextLevelID);
	Event_LevelChange.bIsLoading = true;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event_LevelChange);
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
