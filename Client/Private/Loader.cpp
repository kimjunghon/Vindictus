#include "ClientPch.h"
#include "Loader.h"

#include "Event_Struct.h"

#include "LogoScreen.h"
#include "HUD.h"
#include "OptionMain.h"
#include "OptionController.h"
#include "StateBar.h"
#include "Player.h"
#include "Camera_Target.h"

CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice}
	, m_pDeviceContext { pDeviceContext}
	, m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

unsigned int APIENTRY LoadingMain(void* pArg)
{
	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Loading()))
		return 1;

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);
	
	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL::LOGO:
		hr = Loading_For_Logo_Level();
		break;
	case LEVEL::GAMEPLAY:
		hr = Loading_For_GamePlay_Level();
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	LeaveCriticalSection(&m_CriticalSection);

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Sky_Box.png"), 1))))
		return E_FAIL;	

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_WaterMark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_WaterMark_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark_Back.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_Button"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Button.png"), 1))))
		return E_FAIL;

	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_UIObject_LogoScreen"),
		CLogoScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Back.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Lerp.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Back.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Lerp.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Option_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Back.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Keyboard"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/KeyBoard.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Mouse"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Mouse.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_OptionButton"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Button.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Inventory"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Inventory.png"), 1))))
		return E_FAIL;

	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	Event.fRatio += 0.2f;

	_matrix		PreTransformMatrix = XMMatrixIdentity();

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::NONANIM, "../Bin/Resoruces/Model/Test.json", PreTransformMatrix))))
		return E_FAIL;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_HUD"),
		CHUD::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_StateBar"),
		CStateBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_Option"),
		COptionMain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_OptionController"),
		COptionController::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Target"),
		CCamera_Target::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Event.fRatio += 0.2f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	m_isFinished = true;


	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed Created : CLoader"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
}
