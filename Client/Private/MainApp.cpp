#include "ClientPch.h"

#include "MainApp.h"
#include "Panel.h"
#include "Button.h"
#include "Bar.h"
#include "Level_Loading.h"
#include "Level_GamePlay.h"
#include "Level_Logo.h"
#include "LoadingScreen.h"
#include "LoadingBar.h"
#include "LoadingPoint.h"


CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	ENGINE_DESC EngineDesc{};

	EngineDesc.hInst = g_hInst;
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.eWinMode = WINMODE::WIN;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.iNumLevels = ENUM_CLASS(LEVEL::END);
	
	if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_ForStatic()))
		return E_FAIL;

	if (FAILED(Start_Level(LEVEL::LOGO)))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_LEVEL_CHANGE>(ENUM_CLASS(LEVEL::STATIC), [this](const EVENT_LEVEL_CHANGE& Event) {
		this->Event_LevelChange(Event); });

	return S_OK;
}

void CMainApp::Post_Update()
{
	if (m_bChange_Level)
	{
		if (FAILED(m_pGameInstance->Clear_Resources()))
			MSG_BOX(TEXT("Failed Clear Resrouces"));
		
		CLevel* pNextLevel = Create_NewLevel(m_iChange_Level);

		if (FAILED(m_pGameInstance->Open_Level(m_iChange_Level, pNextLevel)))
			MSG_BOX(TEXT("Failed Open Level"));

		m_bChange_Level = false;
	}
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	_float4		vClearColor = _float4(0.f, 0.f, 1.f, 1.f);

	m_pGameInstance->Render_Begin(&vClearColor);
	m_pGameInstance->Draw();
	m_pGameInstance->Render_End();

	return S_OK;
}

void CMainApp::Event_LevelChange(const EVENT_LEVEL_CHANGE& Event)
{
	m_bChange_Level = true;
	m_iChange_Level = Event.iChange_Level;
	m_bIsLoading = Event.bIsLoading;
}

CLevel* CMainApp::Create_NewLevel(_uint iChangeLevel)
{
	CLevel* pNewLevel = { nullptr };

	if (m_bIsLoading)
	{
		switch (static_cast<LEVEL>(m_iChange_Level))
		{
		case LEVEL::LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
			break;
		case LEVEL::GAMEPLAY:
			pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
			break;
		}
	}
	else
	{
		pNewLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, static_cast<LEVEL>(m_iChange_Level));
		m_iChange_Level = ENUM_CLASS(LEVEL::LOADING);
	}

	return pNewLevel;
}

HRESULT CMainApp::Ready_Prototype_ForStatic()
{
	D3D11_INPUT_ELEMENT_DESC Elements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	/* Prototype_Component_Shader_VtxPosTex */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), Elements, 2))))
		return E_FAIL;

	/* Prototype_Component_VIBuffer_Rect*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_LoadingScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingScreen"),
		CLoadingScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_LoadingBar*/
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingBar"),
		CLoadingBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Panel */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		CPanel::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	
	/* Prototype_UIObject_LoadingPoint */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingPoint"),
		CLoadingPoint::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Ready_Prototype_Component_Texture_LoadingScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingScreen"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingScreen%d.png"), 2))))
		return E_FAIL;

	/* Ready_Prototype_Component_Texture_LoadingBar */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingBar.png"),1))))
		return E_FAIL;

	/* Ready_Prototype_Component_Texture_LoadingBar_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingBack.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_LoadingBar_Point */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Point"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Loading/Point.png"), 1))))
		return E_FAIL;

	/* Ready_Prototype_GameObject_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Button"),
		CButton::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Ready_Prototype_GameObject_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Bar"),
		CBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Start_Level(LEVEL eStartLevelID)
{
	return m_pGameInstance->Open_Level(ENUM_CLASS(LEVEL::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartLevelID));
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
}
