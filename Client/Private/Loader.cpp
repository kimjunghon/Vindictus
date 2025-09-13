#include "ClientPch.h"
#include "Loader.h"

#include "Event_Struct.h"

#include "MapObject.h"

#include "BastardSword.h"
#include "RoundShield.h"
#include "Armor.h"

#include "Effect_Trail.h"
#include "Effect_Static.h"
#include "Effect_Billboard.h"
#include "Effect_Prefab.h"

#include "FireBall.h"
#include "EnergyBall.h"
#include "GavelenRock.h"

#include "Puppy.h"
#include "PuppyBody.h"
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

#include "Camera_Free.h"

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
	case LEVEL::TOWN:
		hr = Loading_For_Town_Level();
		break;
	case LEVEL::FIELD:
		break;
	case LEVEL::QUEEN:
		hr = Loading_For_Queen_Level();
		break;
	case LEVEL::GLASGAVELEN:
		hr = Loading_For_Gavelen_Level();
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

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region TEXTURE
	
#pragma endregion



	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region MODEL
#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region SHADER
#pragma endregion



	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region GAMEOBJECT

#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	Event.fRatio = 1.f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Town_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	//////////////////////////////////////////////////////////////EFFECT//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("이펙트 원형를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Effect_Static"),
		CEffect_Static::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Effect_Billboard"),
		CEffect_Billboard::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Loading_For_Effect("../Bin/Resources/EffectData/LoadFile/TownEffect.json", ENUM_CLASS(LEVEL::TOWN));


	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	///* Prototype_Component_Texture_SwordTrail */
	//if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_SwordTrail"),
	//	CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Effect/heroes_effect_blade_trail_longsword.png"), 1))))
	//	return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region TEXTURE

#pragma endregion

	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));


#pragma region MODEL
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.0f), 0.f);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * RotationMatrix;

	/* Prototype_Component_Model_LightMale_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Head"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Head.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Upper"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Upper_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Lower"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Lower_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Hand"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Hand_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Foot */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Foot"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Foot_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_LightMale_Foot_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_BastardSword */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_BastardSword"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/Sword_Bastard.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_RoundShield */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_RoundShield"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/RoundShield.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Puppy_Gray */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_Puppy_Gray"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Npc/Puppy_Gray.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Puppy_White */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_Puppy_White"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Npc/Puppy_White.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Puppy_Armor */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Model_Puppy_Armor"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Npc/Puppy_Armor.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(Loading_For_MapModel(LEVEL::TOWN, "../Bin/Resources/Map/Town.dat")))
		return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region SHADER
#pragma endregion

	//////////////////////////////////////////////////////////////NAVIGATION//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩중입니다."));

	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));
	/* Prototype_GameObject_Puppy */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Puppy"),
		CPuppy::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Puppy_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Puppy_Body"),
		CPuppyBody::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region GAMEOBJECT


#pragma endregion

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_Queen_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	//////////////////////////////////////////////////////////////EFFECT//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("이펙트 원형를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::QUEEN), TEXT("Prototype_Effect_Static"),
		CEffect_Static::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::QUEEN), TEXT("Prototype_Effect_Billboard"),
		CEffect_Billboard::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Loading_For_Effect("../Bin/Resources/EffectData/LoadFile/QueenEffect.json", ENUM_CLASS(LEVEL::QUEEN));

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region TEXTURE

#pragma endregion

	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));


	if (FAILED(Loading_For_MapModel(LEVEL::QUEEN, "../Bin/Resources/Map/QueenMap.dat")))
		return E_FAIL;

#pragma region MODEL
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.0f), 0.f);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * RotationMatrix;

	/* Prototype_Component_Model_Vampire_Basic_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Vampire_Basic_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Elder_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Vampire_Elder_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire_Elder.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Vampire_Royal_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Vampire_Royal.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_Vampire_Queen_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Model_Queen_Body"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Queen.dat", PreTransformMatrix))))
		return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region SHADER
#pragma endregion

	//////////////////////////////////////////////////////////////NAVIGATION//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩중입니다."));

	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	/* Prototype_GameObject_Vampire_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Basic"),
		CVampire_Basic::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Basic_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Basic_Body"),
		CVampire_Basic_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Elder */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Elder"),
		CVampire_Elder::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Elder_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Elder_Body"),
		CVampire_Elder_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Royal"),
		CVampire_Royal::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Vampire_Royal_Body"),
		CVampire_Royal_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Queen"),
		CQueen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Royal_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Queen_Body"),
		CQueen_Body::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_Projectile_FireBall */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::QUEEN), TEXT("Prototype_Projectile_FireBall"),
		CFireBall::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region GAMEOBJECT

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Gavelen_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	//////////////////////////////////////////////////////////////EFFECT//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("이펙트 원형를 로딩중입니다."));

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Effect_Static"),
		CEffect_Static::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Effect_Billboard"),
		CEffect_Billboard::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Loading_For_Effect("../Bin/Resources/EffectData/LoadFile/GavelenEffect.json", ENUM_CLASS(LEVEL::GLASGAVELEN));

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region TEXTURE

#pragma endregion

	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));


#pragma region MODEL
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(0.f, XMConvertToRadians(180.0f), 0.f);
	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * RotationMatrix;

	/* Prototype_Component_Model_Glasgavelen */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Model_Glasgavelen"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Glasgavelen.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_GlasgavelenBroken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Model_Glasgavelen_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/GlasgavelenBroken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_GavelneSword */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Model_GavelneSword"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/Glasgavelen_Sword.dat", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * RotationMatrix;

	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Component_Model_GavelenRock"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Monster/GavelenRock.dat", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(Loading_For_MapModel(LEVEL::GLASGAVELEN, "../Bin/Resources/Map/GavelenMap.dat")))
		return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region SHADER
#pragma endregion

	//////////////////////////////////////////////////////////////NAVIGATION//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩중입니다."));

	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	/* Prototype_GameObject_Vampire_Basic */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Glasgavelen"),
		CGlasgavelen::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Basic_Body */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_GameObject_Glasgavelen_Body"),
		CGlasgavelenBody::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_GameObject_Vampire_Elder */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_GameObject_Glasgavelen_Sword"),
		CGlasgavelenSword::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* Prototype_Projectile_EnergyBall */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Projectile_EnergyBall"),
		CEnergyBall::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	/* Prototype_Projectile_GavelenRock */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Projectile_GavelenRock"),
		CGavelenRock::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region GAMEOBJECT

#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_MapModel(LEVEL eLevel, const _char* pMapFilePath)
{
	ifstream File(pMapFilePath, ios::binary);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed File Open : MapFile"));
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
	_char szPrototype[MAX_PATH] = "Prototype_Component_Model_";

	for (auto Name : Names)
	{
		_char szFullFilePath[MAX_PATH] = {};

		strcpy_s(szFullFilePath, szDrivePath);
		strcat_s(szFullFilePath, Name.data());
		strcat_s(szFullFilePath, ".dat");

		_matrix PreTransformationMatrix = XMMatrixIdentity();
		CModel* pModel = CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, szFullFilePath, PreTransformationMatrix);
		if (pModel == nullptr)
			return E_FAIL;

		_char szPrototypeTag[MAX_PATH] = {};
		strcpy_s(szPrototypeTag, szPrototype);
		strcat_s(szPrototypeTag, Name.data());

		_tchar szWidePrototypeTag[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, szPrototypeTag, static_cast<_int>(strlen(szPrototypeTag)), szWidePrototypeTag, MAX_PATH);

		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(eLevel), szWidePrototypeTag, pModel)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_Level()
{
	EVENT_PROGRESSBAR Event;
	Event.eType = PROGRESS_TYPE::LOADING;
	Event.fRatio = m_fLoadingRatio;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	//////////////////////////////////////////////////////////////TEXTURE//////////////////////////////////////////////////////////////
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region TEXTURE
	
#pragma endregion



	//////////////////////////////////////////////////////////////MODEL//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다."));
	
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region MODEL


	if (FAILED(Loading_For_GamePlay_ArmorModel()))
		return E_FAIL;

	if (FAILED(Loading_For_MapModel(LEVEL::GAMEPLAY, "../Bin/Resources/Map/QueenMap.dat")))
		return E_FAIL;

#pragma endregion


	//////////////////////////////////////////////////////////////SHADER//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("쉐이더를 로딩중입니다."));
	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region SHADER
#pragma endregion

	//////////////////////////////////////////////////////////////GAMEOBJECT//////////////////////////////////////////////////////////////

	lstrcpy(m_szLoadingText, TEXT("게임오브젝트원형를 로딩중입니다."));

	Event.fRatio += 0.2f;
	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

#pragma region GAMEOBJECT

#pragma region UI_OBJECT

#pragma endregion

	/* Prototype_GameObject_MapObject */
	if(FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_MapObject"),
		CMapObject::Create(m_pDevice, m_pDeviceContext))))
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
#pragma endregion


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));
	Event.fRatio = 1.f;

	m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlay_ArmorModel()
{
	/* Prototype_Component_Model_LightMale_Foot */
	_matrix		PreTransformMatrix = XMMatrixIdentity();
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Foot"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Foot_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Foot_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Foot_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Hand"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Hand_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Hand_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Hand_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Head */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Head"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Head.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Lower"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Lower_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Lower_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Lower_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Upper"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper.dat", PreTransformMatrix))))
		return E_FAIL;

	/* Prototype_Component_Model_LightMale_Upper_Broken */
	if (FAILED(m_pGameInstance->Add_Prototype(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Model_LightMale_Upper_Broken"),
		CModel::Create(m_pDevice, m_pDeviceContext, MODEL_TYPE::INFILE, "../Bin/Resources/Models/Player/LightMale_Upper_Broken.dat", PreTransformMatrix))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Effect(const _char* pFilePath, _uint iLevel)
{
	ifstream File(pFilePath);

	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("EffectFile") && Doc["EffectFile"].IsArray())
	{
		const auto& Effects = Doc["EffectFile"].GetArray();

		for(auto& Effect : Effects)
		{
			EFFECT_TYPE eType = {};

			if (Effect.HasMember("Type") && Effect["Type"].IsInt())
				eType = static_cast<EFFECT_TYPE>(Effect["Type"].GetInt());

			string strFilePath = {};

			if (Effect.HasMember("FilePath") && Effect["FilePath"].IsString())
				strFilePath = (Effect["FilePath"].GetString());

			switch (eType)
			{
			case EFFECT_TYPE::STATIC:
				Load_Static_Effect(strFilePath.c_str(), iLevel);
				break;

			case EFFECT_TYPE::BILLBAORD:
				Load_Billboard_Effect(strFilePath.c_str(), iLevel);
				break;

			case EFFECT_TYPE::PREFAB:
				Load_Effect_Prefab(strFilePath.c_str(), iLevel);
				break;
			case EFFECT_TYPE::TRAIL:
				Load_Effect_Trail(strFilePath.c_str(), iLevel);
				break;
			}
		}
	}


	return S_OK;
}

HRESULT CLoader::Load_Static_Effect(const _char* pFilePath, _uint iLevel)
{
	ifstream File(pFilePath);

	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("Effect") && Doc["Effect"].IsObject())
	{
		const Value& Effect = Doc["Effect"];

		_tchar EffectName[MAX_PATH] = {};

		if (Effect.HasMember("Name") && Effect["Name"].IsString())
		{
			string Name = Effect["Name"].GetString();

			MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), EffectName, static_cast<_int>(Name.size()));
		}

		string strTexturePath = {};
		_uint iNumTextures = {};

		if (Effect.HasMember("Texture") && Effect["Texture"].IsString())
			strTexturePath = Effect["Texture"].GetString();

		if (Effect.HasMember("NumTextures") && Effect["NumTextures"].IsInt())
			iNumTextures = Effect["NumTextures"].GetInt();

		_wstring TextureTag = TEXT("Prototype_Component_Texture_");
		_tchar TextureFilePath[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, strTexturePath.c_str(), static_cast<_int>(strTexturePath.size()), TextureFilePath, static_cast<_int>(strTexturePath.size()));

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, TextureTag + EffectName,
			CTexture::Create(m_pDevice, m_pDeviceContext, TextureFilePath, iNumTextures))))
			return E_FAIL;

		CVIBuffer_Rect_Instance::RECT_INSTANCE_DESC RectDesc = {};

		if (Effect.HasMember("ColorR") && Effect["ColorR"].IsFloat() &&
			Effect.HasMember("ColorG") && Effect["ColorG"].IsFloat() &&
			Effect.HasMember("ColorB") && Effect["ColorB"].IsFloat())
		{
			RectDesc.vSourceColor.x = Effect["ColorR"].GetFloat();
			RectDesc.vSourceColor.y = Effect["ColorG"].GetFloat();
			RectDesc.vSourceColor.z = Effect["ColorB"].GetFloat();
		}

		if (Effect.HasMember("NumInstance") && Effect["NumInstance"].IsInt())
			RectDesc.iNumInstance = Effect["NumInstance"].GetInt();

		if (Effect.HasMember("CenterX") && Effect["CenterX"].IsFloat() &&
			Effect.HasMember("CenterY") && Effect["CenterY"].IsFloat() &&
			Effect.HasMember("CenterZ") && Effect["CenterZ"].IsFloat())
		{
			RectDesc.vCenter.x = Effect["CenterX"].GetFloat();
			RectDesc.vCenter.y = Effect["CenterY"].GetFloat();
			RectDesc.vCenter.z = Effect["CenterZ"].GetFloat();
		}

		if (Effect.HasMember("RangeX") && Effect["RangeX"].IsFloat() &&
			Effect.HasMember("RangeY") && Effect["RangeY"].IsFloat() &&
			Effect.HasMember("RangeZ") && Effect["RangeZ"].IsFloat())
		{
			RectDesc.vRange.x = Effect["RangeX"].GetFloat();
			RectDesc.vRange.y = Effect["RangeY"].GetFloat();
			RectDesc.vRange.z = Effect["RangeZ"].GetFloat();
		}

		if (Effect.HasMember("SizeX") && Effect["SizeX"].IsFloat() &&
			Effect.HasMember("SizeY") && Effect["SizeY"].IsFloat())
		{
			RectDesc.vSize.x = Effect["SizeX"].GetFloat();
			RectDesc.vSize.y = Effect["SizeY"].GetFloat();
		}

		if (Effect.HasMember("PivotX") && Effect["PivotX"].IsFloat() &&
			Effect.HasMember("PivotY") && Effect["PivotY"].IsFloat() &&
			Effect.HasMember("PivotZ") && Effect["PivotZ"].IsFloat())
		{
			RectDesc.vPivot.x = Effect["PivotX"].GetFloat();
			RectDesc.vPivot.y = Effect["PivotY"].GetFloat();
			RectDesc.vPivot.z = Effect["PivotZ"].GetFloat();
		}

		if (Effect.HasMember("SpeedX") && Effect["SpeedX"].IsFloat() &&
			Effect.HasMember("SpeedY") && Effect["SpeedY"].IsFloat())
		{
			RectDesc.vSpeed.x = Effect["SpeedX"].GetFloat();
			RectDesc.vSpeed.y = Effect["SpeedY"].GetFloat();
		}

		if (Effect.HasMember("LifeTimeX") && Effect["LifeTimeX"].IsFloat() &&
			Effect.HasMember("LifeTimeY") && Effect["LifeTimeY"].IsFloat())
		{
			RectDesc.vLifeTime.x = Effect["LifeTimeX"].GetFloat();
			RectDesc.vLifeTime.y = Effect["LifeTimeY"].GetFloat();
		}

		if (Effect.HasMember("Loop") && Effect["Loop"].IsBool())
			RectDesc.IsLoop = Effect["Loop"].GetBool();

		if (Effect.HasMember("Circle") && Effect["Circle"].IsBool())
			RectDesc.IsCircle = Effect["Circle"].GetBool();

		if (Effect.HasMember("AngleX") && Effect["AngleX"].IsFloat() &&
			Effect.HasMember("AngleY") && Effect["AngleY"].IsFloat())
		{
			RectDesc.vAngle.x = Effect["AngleX"].GetFloat();
			RectDesc.vAngle.y = Effect["AngleY"].GetFloat();
		}


		if (Effect.HasMember("FX_Type") && Effect["FX_Type"].IsInt())
			RectDesc.eType = static_cast<CVIBuffer_Rect_Instance::FX_RECT_TYPE>(Effect["FX_Type"].GetInt());

		_wstring VIBufferTag = TEXT("Prototype_Component_EffectBuffer_");

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, VIBufferTag + EffectName,
			CVIBuffer_Rect_Instance::Create(m_pDevice, m_pDeviceContext, &RectDesc))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Billboard_Effect(const _char* pFilePath, _uint iLevel)
{
	ifstream File(pFilePath);

	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("Effect") && Doc["Effect"].IsObject())
	{
		const Value& Effect = Doc["Effect"];

		_tchar EffectName[MAX_PATH] = {};

		if (Effect.HasMember("Name") && Effect["Name"].IsString())
		{
			string Name = Effect["Name"].GetString();

			MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), EffectName, static_cast<_int>(Name.size()));
		}

		string strTexturePath = {};
		_uint iNumTextures = {};

		if (Effect.HasMember("Texture") && Effect["Texture"].IsString())
			strTexturePath = Effect["Texture"].GetString();

		if (Effect.HasMember("NumTextures") && Effect["NumTextures"].IsInt())
			iNumTextures = Effect["NumTextures"].GetInt();

		_wstring TextureTag = TEXT("Prototype_Component_Texture_");
		_tchar TextureFilePath[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, strTexturePath.c_str(), static_cast<_int>(strTexturePath.size()), TextureFilePath, static_cast<_int>(strTexturePath.size()));

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, TextureTag + EffectName,
			CTexture::Create(m_pDevice, m_pDeviceContext, TextureFilePath, iNumTextures))))
			return E_FAIL;

		CVIBuffer_Point_Instance::POINT_INSTANCE_DESC PointDesc = {};

		if (Effect.HasMember("ColorR") && Effect["ColorR"].IsFloat() &&
			Effect.HasMember("ColorG") && Effect["ColorG"].IsFloat() &&
			Effect.HasMember("ColorB") && Effect["ColorB"].IsFloat())
		{
			PointDesc.vSourceColor.x = Effect["ColorR"].GetFloat();
			PointDesc.vSourceColor.y = Effect["ColorG"].GetFloat();
			PointDesc.vSourceColor.z = Effect["ColorB"].GetFloat();
		}

		if (Effect.HasMember("NumInstance") && Effect["NumInstance"].IsInt())
			PointDesc.iNumInstance = Effect["NumInstance"].GetInt();

		if (Effect.HasMember("CenterX") && Effect["CenterX"].IsFloat() &&
			Effect.HasMember("CenterY") && Effect["CenterY"].IsFloat() &&
			Effect.HasMember("CenterZ") && Effect["CenterZ"].IsFloat())
		{
			PointDesc.vCenter.x = Effect["CenterX"].GetFloat();
			PointDesc.vCenter.y = Effect["CenterY"].GetFloat();
			PointDesc.vCenter.z = Effect["CenterZ"].GetFloat();
		}

		if (Effect.HasMember("RangeX") && Effect["RangeX"].IsFloat() &&
			Effect.HasMember("RangeY") && Effect["RangeY"].IsFloat() &&
			Effect.HasMember("RangeZ") && Effect["RangeZ"].IsFloat())
		{
			PointDesc.vRange.x = Effect["RangeX"].GetFloat();
			PointDesc.vRange.y = Effect["RangeY"].GetFloat();
			PointDesc.vRange.z = Effect["RangeZ"].GetFloat();
		}

		if (Effect.HasMember("SizeX") && Effect["SizeX"].IsFloat() &&
			Effect.HasMember("SizeY") && Effect["SizeY"].IsFloat())
		{
			PointDesc.vSize.x = Effect["SizeX"].GetFloat();
			PointDesc.vSize.y = Effect["SizeY"].GetFloat();
		}

		if (Effect.HasMember("PivotX") && Effect["PivotX"].IsFloat() &&
			Effect.HasMember("PivotY") && Effect["PivotY"].IsFloat() &&
			Effect.HasMember("PivotZ") && Effect["PivotZ"].IsFloat())
		{
			PointDesc.vPivot.x = Effect["PivotX"].GetFloat();
			PointDesc.vPivot.y = Effect["PivotY"].GetFloat();
			PointDesc.vPivot.z = Effect["PivotZ"].GetFloat();
		}

		if (Effect.HasMember("SpeedX") && Effect["SpeedX"].IsFloat() &&
			Effect.HasMember("SpeedY") && Effect["SpeedY"].IsFloat())
		{
			PointDesc.vSpeed.x = Effect["SpeedX"].GetFloat();
			PointDesc.vSpeed.y = Effect["SpeedY"].GetFloat();
		}

		if (Effect.HasMember("LifeTimeX") && Effect["LifeTimeX"].IsFloat() &&
			Effect.HasMember("LifeTimeY") && Effect["LifeTimeY"].IsFloat())
		{
			PointDesc.vLifeTime.x = Effect["LifeTimeX"].GetFloat();
			PointDesc.vLifeTime.y = Effect["LifeTimeY"].GetFloat();
		}

		if (Effect.HasMember("Loop") && Effect["Loop"].IsBool())
			PointDesc.IsLoop = Effect["Loop"].GetBool();


		if (Effect.HasMember("Circle") && Effect["Circle"].IsBool())
			PointDesc.IsCircle = Effect["Circle"].GetBool();

		if (Effect.HasMember("AngleX") && Effect["AngleX"].IsFloat() &&
			Effect.HasMember("AngleY") && Effect["AngleY"].IsFloat())
		{
			PointDesc.vAngle.x = Effect["AngleX"].GetFloat();
			PointDesc.vAngle.y = Effect["AngleY"].GetFloat();
		}


		_wstring VIBufferTag = TEXT("Prototype_Component_EffectBuffer_");

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, VIBufferTag + EffectName,
			CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, &PointDesc))))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Effect_Prefab(const _char* pFilePath, _uint iLevel)
{
	ifstream File(pFilePath);

	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	_tchar EffectName[MAX_PATH] = {};

	if (Doc.HasMember("PrefabName") && Doc["PrefabName"].IsString())
	{
		string Name = Doc["PrefabName"].GetString();

		MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), EffectName, static_cast<_int>(Name.size()));
	}


	_wstring strEffectTag = TEXT("Prototype_Effect_");

	if (Doc.HasMember("PrefabEffect") && Doc["PrefabEffect"].IsArray())
	{
		const Value& Effects = Doc["PrefabEffect"].GetArray();

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, strEffectTag + EffectName,
			CEffect_Prefab::Create(m_pDevice, m_pDeviceContext, Effects))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CLoader::Load_Effect_Trail(const _char* pFilePath, _uint iLevel)
{
	ifstream File(pFilePath);

	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed Open"));
		return E_FAIL;
	}

	IStreamWrapper FileWrap(File);

	Document Doc;
	Doc.ParseStream(FileWrap);

	if (Doc.HasParseError())
	{
		MSG_BOX(TEXT("Failed ParseStream"));
		return E_FAIL;
	}

	if (Doc.HasMember("Effect") && Doc["Effect"].IsObject())
	{
		const Value& Effect = Doc["Effect"];

		_tchar EffectName[MAX_PATH] = {};

		if (Effect.HasMember("Name") && Effect["Name"].IsString())
		{
			string Name = Effect["Name"].GetString();

			MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), EffectName, static_cast<_int>(Name.size()));
		}

		string strTexturePath = {};
		_uint iNumTextures = {};

		if (Effect.HasMember("Texture") && Effect["Texture"].IsString())
			strTexturePath = Effect["Texture"].GetString();

		_wstring TextureTag = TEXT("Prototype_Component_Texture_");
		_tchar TextureFilePath[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, strTexturePath.c_str(), static_cast<_int>(strTexturePath.size()), TextureFilePath, static_cast<_int>(strTexturePath.size()));

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, TextureTag + EffectName,
			CTexture::Create(m_pDevice, m_pDeviceContext, TextureFilePath, 1))))
			return E_FAIL;

		TRAIL_TYPE eType = {};

		if (Effect.HasMember("TrailType") && Effect["TrailType"].IsInt())
			eType = static_cast<TRAIL_TYPE>(Effect["TrailType"].GetInt());

		_float3 vRotate = {};

		if (Effect.HasMember("RotateX") && Effect["RotateX"].IsFloat() &&
			Effect.HasMember("RotateY") && Effect["RotateY"].IsFloat() &&
			Effect.HasMember("RotateZ") && Effect["RotateZ"].IsFloat())
		{
			vRotate.x = Effect["RotateX"].GetFloat();
			vRotate.y = Effect["RotateY"].GetFloat();
			vRotate.z = Effect["RotateZ"].GetFloat();
		}

		_vector vRotationQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(vRotate.x), XMConvertToRadians(vRotate.y), XMConvertToRadians(vRotate.z));
		
		_matrix RotateMatrix = XMMatrixRotationQuaternion(vRotationQuaternion);

		_float3 vColor = {};

		if (Effect.HasMember("ColorR") && Effect["ColorR"].IsFloat() &&
			Effect.HasMember("ColorG") && Effect["ColorG"].IsFloat() &&
			Effect.HasMember("ColorB") && Effect["ColorB"].IsFloat())
		{
			vColor.x = Effect["ColorR"].GetFloat();
			vColor.y = Effect["ColorG"].GetFloat();
			vColor.z = Effect["ColorB"].GetFloat();
		}

		_wstring strEffectTag = TEXT("Prototype_Effect_Trail_");

		if (FAILED(m_pGameInstance->Add_Prototype(iLevel, strEffectTag + EffectName,
			CEffect_Trail::Create(m_pDevice, m_pDeviceContext, eType, RotateMatrix, vColor))))
			return E_FAIL;

	}
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
