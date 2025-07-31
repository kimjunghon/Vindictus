#include "ClientPch.h"
#include "Loader.h"

#include "Event_Struct.h"

#include "LogoScreen.h"
#include "HUD.h"
#include "OptionMain.h"
#include "OptionController.h"
#include "StateBar.h"
#include "MapObject.h"

#include "Camera_Target.h"
#include "PlayerPawn.h"
#include "PlayerBody.h"
#include "BastardSword.h"
#include "RoundShield.h"
#include "Armor.h"

#include "Glasgavelen.h"
#include "GlasgavelenBody.h"
#include "GlasgavelenSword.h"
#include "Vampire_Basic.h"
#include "Vampire_Basic_Body.h"
#include "Vampire_Elder.h"
#include "Vampire_Elder_Body.h"
#include "Vampire_Royal.h"
#include "Vampire_Royal_Body.h"
#include "Queen.h"
#include "Queen_Body.h"

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

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region TEXTURE
	/* Prototype_Component_Texture_SkyBox */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Sky_Box.png"), 1))))
		return E_FAIL;	

	/* Prototype_Component_Texture_Logo_WaterMark */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_WaterMark"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_WaterMark_Back */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_WaterMark_Back"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Watermark_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_Logo_Button */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Logo_Button"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/Logo/Logo_Button.png"), 1))))
		return E_FAIL;
#pragma endregion



	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region MODEL
#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region SHADER
#pragma endregion



	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region GAMEOBJECT

	/* Prototype_UIObject_LogoScreen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_UIObject_LogoScreen"),
		CLogoScreen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

#pragma endregion

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

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region TEXTURE

#pragma region UI_TEXTURE
	/* Prototype_Component_Texture_GamePlay_Back_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Lerp_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar_Lerp.png"), 1))))
		return E_FAIL;
	
	/* Prototype_Component_Texture_GamePlay_PlayerHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerHpBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerHpBar.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Back_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Back_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Lerp_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Lerp_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar_Lerp.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_PlayerStaminaBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_PlayerStaminaBar"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/PlayerStaminaBar.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Option_Background */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Option_Background"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Back.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Keyboard */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Keyboard"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/KeyBoard.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Mouse"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Mouse.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_OptionButton */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_OptionButton"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Option_Button.png"), 1))))
		return E_FAIL;

	/* Prototype_Component_Texture_GamePlay_Inventory */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_GamePlay_Inventory"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI/GamePlay/Inventory.png"), 1))))
		return E_FAIL;
#pragma endregion
	
#pragma endregion



	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region MODEL

	/* Prototype_Component_Model_Player */
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.0f), 0.f);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * RotationMatrix;
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/Piona.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_BastardSword */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_BastardSword"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/Sword_Bastard.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_RoundShield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_RoundShield"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/RoundShield.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Glasgavelen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Glasgavelen"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Glasgavelen.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_RoundShield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_GavelneSword"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Glasgavelen_Sword.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Basic_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Vampire_Basic_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Elder_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Vampire_Elder_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire_Elder.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Vampire_Royal_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire_Royal.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Queen_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_Queen_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Monster/Queen.dat", PreTransformMatrix))))
		return E_FAIL;


	if (FAILED(Loading_For_GamePlay_ArmorModel()))
		return E_FAIL;

	if (FAILED(Loading_For_GamePlay_MapModel()))
		return E_FAIL;

#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region SHADER
#pragma endregion

	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

#pragma region GAMEOBJECT

#pragma region UI_OBJECT
	/* Prototype_UIObject_HUD */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_HUD"),
		CHUD::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_StateBar */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_StateBar"),
		CStateBar::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_Option */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_Option"),
		COptionMain::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_UIObject_OptionController */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_OptionController"),
		COptionController::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

	/* Prototype_GameObject_Camera_Target */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Camera_Target"),
		CCamera_Target::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_MapObject */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Player_Body */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Player_Body"),
		CPlayerBody::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_PlayerPawn */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_PlayerPawn"),
		CPlayerPawn::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_BastardSword */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_BastardSword"),
		CBastardSword::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_RoundShield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_RoundShield"),
		CRoundShield::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Armor"),
		CArmor::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Glasgavelen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen"),
		CGlasgavelen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Glasgavelen_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen_Body"),
		CGlasgavelenBody::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Glasgavelen_Sword */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Glasgavelen_Sword"),
		CGlasgavelenSword::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Basic"),
		CVampire_Basic::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Basic_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Basic_Body"),
		CVampire_Basic_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Elder */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Elder"),
		CVampire_Elder::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Elder_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Elder_Body"),
		CVampire_Elder_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Royal"),
		CVampire_Royal::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Vampire_Royal_Body"),
		CVampire_Royal_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Queen"),
		CQueen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Queen_Body"),
		CQueen_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_MapModel()
{
	ifstream File("../Bin/Resources/BossMap1.dat", ios::binary);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed File Open : BossMap1"));
		return E_FAIL;
	}

	size_t iNumObjects = {};
	File.read(reinterpret_cast<_char*>(&iNumObjects), sizeof(size_t));
	
	unordered_set<string> Names;

	for (size_t i = 0; i < iNumObjects; i++)
	{
		size_t iNameLength = {};
		_char szName[MAX_PATH] = {};

		File.read(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
		File.read(szName, sizeof(_char) * iNameLength);
		File.seekg(sizeof(_float4x4), ios::cur);

		Names.insert(szName);
	}

	File.close();
	
	_char szDrivePath[MAX_PATH] = "../Bin/Resources/Models/MapObject/";
	_char szPrototype[MAX_PATH] = "Prototype_GameObject_Map_";

	for (auto Name : Names)
	{
		_char szFullFilePath[MAX_PATH] = {};

		strcpy_s(szFullFilePath, szDrivePath);
		strcat_s(szFullFilePath, Name.data());
		strcat_s(szFullFilePath, ".dat");

		_matrix PreTransformationMatrix = XMMatrixIdentity();
		CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, szFullFilePath, PreTransformationMatrix);
		if (pModel == nullptr)
			return E_FAIL;

		_char szPrototypeTag[MAX_PATH] = {};
		strcpy_s(szPrototypeTag, szPrototype);
		strcat_s(szPrototypeTag, Name.data());

		_tchar szWidePrototypeTag[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, szPrototypeTag, static_cast<_int>(strlen(szPrototypeTag)), szWidePrototypeTag, MAX_PATH);

		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), szWidePrototypeTag, pModel)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_ArmorModel()
{
	/* Prototype_Component_Model_LightMale_Foot */
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Foot"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Foot_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Foot_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Hand"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Hand_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Head"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Head.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Lower"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Lower_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Upper"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Upper_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODELTYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

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
