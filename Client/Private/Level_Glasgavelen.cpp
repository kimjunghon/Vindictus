#include "ClientPch.h"
#include "Level_Glasgavelen.h"

#include "Map.h"
#include "PlayerPawn.h"
#include "Pool_Instance.h"
#include "Effect_Distortion.h"
#include "Camera_CS.h"
#include "TriggerBox.h"

CLevel_Glasgavelen::CLevel_Glasgavelen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
	, m_pPool_Instance{ CPool_Instance::GetInstance() }
{
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CLevel_Glasgavelen::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_GameObjectToJson()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	if (FAILED(Ready_PoolingMonster()))
		return E_FAIL;

	if (FAILED(Ready_PoolingProjectile()))
		return E_FAIL;
	
	if (FAILED(Ready_Map(TEXT("Layer_GameObject"))))
		return E_FAIL;

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_TriggerBox()))
		return E_FAIL;

	m_pPool_Instance->BeginRoomSpawn(0);

	if (FAILED(m_pGameInstance->Play_Sound(TEXT("Gavelen_Bgm"), ENUM_CLASS(SOUND_CHANNEL::BGM), 0.3f, true)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Glasgavelen::Update(_float fTimeDelta)
{

}

HRESULT CLevel_Glasgavelen::Render()
{
	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("DIRECTONAL"), LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_TriggerBox()
{
	CTriggerBox::TRIGGER_DESC Trigger_Desc = {};

	Trigger_Desc.vPosition = XMVectorSet(10.f, 0.f, 10.f, 1.f);
	Trigger_Desc.vSize = _float3(20.f, 20.f, 20.f);
	Trigger_Desc.Callback = [this]() { EVENT_GAVELEN_CUTSCENE Event = {}; m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::NONSTATIC), Event); };
	
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_TriggerBox"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_TriggerBox"), &Trigger_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_GameObjectToJson()
{
	ifstream File("../Bin/Resources/Map/GavelenBat.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed GavelenBat Open"));
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

	if (Doc.HasMember("Player") && Doc["Player"].IsObject())
	{
		const Value& Player = Doc["Player"];

		if (FAILED(Ready_Player(Player)))
			return E_FAIL;
	}

	if (Doc.HasMember("RoomSpawnDatas") && Doc["RoomSpawnDatas"].IsArray())
	{
		const Value& RoomSpawnDatas = Doc["RoomSpawnDatas"];

		if (FAILED(m_pPool_Instance->Ready_Spawn_Data(RoomSpawnDatas)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_Player(const Value& Player)
{
	_int iCellIndex = { };
	_float3 vPosition = {};

	if (Player.HasMember("CellIndex") && Player["CellIndex"].IsInt())
		iCellIndex = Player["CellIndex"].GetInt();

	if (Player.HasMember("PositionX") && Player["PositionX"].IsFloat()
		&& Player.HasMember("PositionY") && Player["PositionY"].IsFloat()
		&& Player.HasMember("PositionZ") && Player["PositionZ"].IsFloat())
	{
		vPosition = _float3(Player["PositionX"].GetFloat(), Player["PositionY"].GetFloat(), Player["PositionZ"].GetFloat());
	}

	CPlayerPawn::PLAYER_DESC PlayerDesc = {};
	PlayerDesc.fSpeedPerSec = 10.f;
	PlayerDesc.fRotationPerSec = XMConvertToRadians(90.f);
	PlayerDesc.iCellIndex = iCellIndex;
	PlayerDesc.vPosition = vPosition;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_PlayerPawn"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Player"), &PlayerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_Camera()
{
	CCamera_CS::CAMERA_CS_DESC CS_Desc = {};
	CS_Desc.vEye = _float4(0.f, 30.f, -150.f, 1.f);
	CS_Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CS_Desc.fFov = XMConvertToRadians(60.0f);
	CS_Desc.fNear = 0.1f;
	CS_Desc.fFar = 3000.f;
	CS_Desc.fSpeedPerSec = 0.f;
	CS_Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	CS_Desc.pFilePath = "../Bin/Resources/Cutscene/realrealreal.dat";
	
	if (FAILED(m_pGameInstance->Add_CameraToManager(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Camera_Cutscene"), TEXT("Gavelen_CS_Camera"), nullptr, &CS_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_Effect()
{
	ifstream File("../Bin/Resources/EffectData/LoadFile/GavelenEffectPool.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed GavelenEffectPool Open"));
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

	if (Doc.HasMember("EffectPool") && Doc["EffectPool"].IsArray())
	{
		const auto& Effects = Doc["EffectPool"].GetArray();

		for (auto& Effect : Effects)
		{

			CEffect::EFFECT_DESC EffectDesc = {};

			_tchar EffectName[MAX_PATH] = {};

			if (Effect.HasMember("Name") && Effect["Name"].IsString())
			{
				string Name = Effect["Name"].GetString();

				MultiByteToWideChar(CP_UTF8, 0, Name.c_str(), static_cast<_int>(Name.size()), EffectName, static_cast<_int>(Name.size()));
			}

			_wstring strEffectTag = {};

			EFFECT_TYPE eType = {};

			if (Effect.HasMember("Type") && Effect["Type"].IsInt())
			{
				eType = static_cast<EFFECT_TYPE>(Effect["Type"].GetInt());
				switch (eType)
				{
				case EFFECT_TYPE::STATIC:
					strEffectTag = TEXT("Prototype_Effect_Static");
					break;
				case EFFECT_TYPE::BILLBAORD:
					strEffectTag = TEXT("Prototype_Effect_Billboard");
					break;
				case EFFECT_TYPE::PREFAB:
					strEffectTag = TEXT("Prototype_Effect_");
					strEffectTag = strEffectTag + EffectName;
					break;
				case EFFECT_TYPE::TRAIL:
					strEffectTag = TEXT("Prototype_Effect_Trail_");
					strEffectTag = strEffectTag + EffectName;
					break;
				case EFFECT_TYPE::DISTORTION:
					strEffectTag = TEXT("Prototype_Effect_Distortion");
					break;
				}
			}

			_uint iPassIndex = {};
			_uint iNumPool = {};

			if (Effect.HasMember("Pass") && Effect["Pass"].IsInt())
				iPassIndex = Effect["Pass"].GetInt();

			if (Effect.HasMember("NumPool") && Effect["NumPool"].IsInt())
				iNumPool = Effect["NumPool"].GetInt();


			if (eType == EFFECT_TYPE::DISTORTION)
			{
				CEffect_Distortion::DISTORTION_DESC DistortionDesc = {};
				DistortionDesc.strEffectName = EffectName;
				DistortionDesc.iLevel = ENUM_CLASS(LEVEL::GLASGAVELEN);
				DistortionDesc.iPassIndex = iPassIndex;

				_bool IsMask = {};

				if (Effect.HasMember("IsMask") && Effect["IsMask"].IsBool())
					IsMask = Effect["IsMask"].GetBool();

				DistortionDesc.Is_Masking = IsMask;

				if (IsMask)
				{
					_tchar MaskTextureName[MAX_PATH] = {};

					if (Effect.HasMember("MaskEffectName") && Effect["MaskEffectName"].IsString())
					{
						string MaskName = Effect["MaskEffectName"].GetString();

						MultiByteToWideChar(CP_UTF8, 0, MaskName.c_str(), static_cast<_int>(MaskName.size()), MaskTextureName, static_cast<_int>(MaskName.size()));

					}

					DistortionDesc.strMaskTextureName = MaskTextureName;
				}

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::GLASGAVELEN), EffectName, strEffectTag, &DistortionDesc);

			}
			else
			{
				CEffect::EFFECT_DESC EffectDesc = {};

				EffectDesc.strEffectName = EffectName;
				EffectDesc.iPassIndex = iPassIndex;
				EffectDesc.iLevel = ENUM_CLASS(LEVEL::GLASGAVELEN);

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::GLASGAVELEN), EffectName, strEffectTag, &EffectDesc);

			}

		}
	}

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_PoolingMonster()
{
	ifstream File("../Bin/Resources/Map/GavelenMap_PoolingList.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed GavelenMap_PoolingList Open"));
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

	if (Doc.HasMember("MonsterPool") && Doc["MonsterPool"].IsArray())
	{
		const Value& MonsterPool = Doc["MonsterPool"];

		if (FAILED(m_pPool_Instance->Ready_MonsterPool(MonsterPool)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_PoolingProjectile()
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 150.f;
	GameObjectDesc.fRotationPerSec = 0.f;

	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(m_pPool_Instance->Add_ProjectilePool(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Projectile_EnergyBall"), TEXT("EnergyBall"), &GameObjectDesc)))
			return E_FAIL;
	}

	GameObjectDesc.fSpeedPerSec = 400.f;

	for (_uint i = 0; i < 5; i++)
	{
		if (FAILED(m_pPool_Instance->Add_ProjectilePool(ENUM_CLASS(LEVEL::GLASGAVELEN), TEXT("Prototype_Projectile_GavelenRock"), TEXT("GavelenRock"), &GameObjectDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Glasgavelen::Ready_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC MapDesc = {};
	MapDesc.strMapFilePath = "../Bin/Resources/Map/GavelenMap.dat";
	MapDesc.eLevel = LEVEL::GLASGAVELEN;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Map"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), strLayerTag, &MapDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Glasgavelen* CLevel_Glasgavelen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Glasgavelen* pInstance = new CLevel_Glasgavelen(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Glasgavelen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Glasgavelen::Free()
{
	__super::Free();

	Safe_Release(m_pPool_Instance);
}
