#include "ClientPch.h"
#include "Level_Queen.h"

#include "Map.h"
#include "PlayerPawn.h"
#include "Pool_Instance.h"
#include "Effect_Distortion.h"
#include "SkyBox.h"

CLevel_Queen::CLevel_Queen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
	, m_pPool_Instance { CPool_Instance::GetInstance()}
{
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CLevel_Queen::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_Sky()))
		return E_FAIL;

	if (FAILED(Ready_GameObjectToJson()))
		return E_FAIL;

	if (FAILED(Ready_PoolingMonster()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	if (FAILED(Ready_PoolingProjectile()))
		return E_FAIL;

	if (FAILED(Ready_Map(TEXT("Layer_GameObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Queen::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	if (m_pGameInstance->Get_KeyDown(DIK_F1))
	{
		m_pPool_Instance->SpawnRoom(2, 2);
//		m_pPool_Instance->BeginRoomSpawn(0);
	}
	if (m_pGameInstance->Get_KeyDown(DIK_F2))
	{
		m_pPool_Instance->SpawnRoom(0, 0);
		//		m_pPool_Instance->BeginRoomSpawn(0);
	}
	if (m_pGameInstance->Get_KeyDown(DIK_F3))
	{
		m_pPool_Instance->SpawnRoom(3, 0);
		//		m_pPool_Instance->BeginRoomSpawn(0);
	}
	if (m_pGameInstance->Get_KeyDown(DIK_T))
	{
		m_pPool_Instance->WaveEnd();
	}
#endif
	if (m_pGameInstance->Get_KeyDown(DIK_RETURN))
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::GLASGAVELEN);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
	}
}

HRESULT CLevel_Queen::Render()
{
	return S_OK;
}

HRESULT CLevel_Queen::Ready_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("DIRECTONAL"), LightDesc)))
		return E_FAIL;


	SHADOW_LIGHT_DESC	ShadowLightDesc = {};
	ShadowLightDesc.vDirection = _float4(-0.5f, -1.f, 0.5f, 0.f);
	ShadowLightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	ShadowLightDesc.fDistance = 2500.f;
	ShadowLightDesc.fFar = 5000.f;
	ShadowLightDesc.fNear = 1500.f;

	if (FAILED(m_pGameInstance->Update_ShadowLight(ShadowLightDesc)))
		return E_FAIL;

	m_pGameInstance->Update_ShadowLight(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Queen::Ready_Sky()
{
	CSkyBox::SKYBOX_DESC SkyDesc = {};
	SkyDesc.iTextureLevel = ENUM_CLASS(LEVEL::QUEEN);
	SkyDesc.strTextureTag = TEXT("Prototype_Component_Texture_Battle_Sky");

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_SkyBox"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Sky"), &SkyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Queen::Ready_GameObjectToJson()
{
	ifstream File("../Bin/Resources/Map/QueenBat.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed QueenBat Open"));
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

	File.close();
	
	return S_OK;
}

HRESULT CLevel_Queen::Ready_Player(const Value& Player)
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

HRESULT CLevel_Queen::Ready_Effect()
{
	ifstream File("../Bin/Resources/EffectData/LoadFile/QueenEffectPool.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed QueenEffectPool Open"));
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
				DistortionDesc.iLevel = ENUM_CLASS(LEVEL::QUEEN);
				DistortionDesc.iPassIndex = iPassIndex;

				_bool IsMask = {};

				if (Effect.HasMember("IsMask") && Effect["IsMask"].IsBool())
					IsMask = Effect["IsMask"].GetBool();

				if(IsMask)
				{
					_tchar MaskTextureName[MAX_PATH] = {};

					if (Effect.HasMember("MaskEffectName") && Effect["MaskEffectName"].IsString())
					{
						string MaskName = Effect["MaskEffectName"].GetString();

						MultiByteToWideChar(CP_UTF8, 0, MaskName.c_str(), static_cast<_int>(MaskName.size()), MaskTextureName, static_cast<_int>(MaskName.size()));

					}

					DistortionDesc.strMaskTextureName = MaskTextureName;

				}

				DistortionDesc.Is_Masking = IsMask;

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::QUEEN), EffectName, strEffectTag, &DistortionDesc);

			}
			else
			{
				CEffect::EFFECT_DESC EffectDesc = {};

				EffectDesc.strEffectName = EffectName;
				EffectDesc.iPassIndex = iPassIndex;
				EffectDesc.iLevel = ENUM_CLASS(LEVEL::QUEEN);

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::QUEEN), EffectName, strEffectTag, &EffectDesc);
				
			}

		}
	}

	return S_OK;
}

HRESULT CLevel_Queen::Ready_PoolingMonster()
{
	ifstream File("../Bin/Resources/Map/QueenMap_PoolingList.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed QueenMap_PoolingList Open"));
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

HRESULT CLevel_Queen::Ready_PoolingProjectile()
{
	CGameObject::GAMEOBJECT_DESC GameObjectDesc = {};
	GameObjectDesc.fSpeedPerSec = 300.f;
	GameObjectDesc.fRotationPerSec = 0.f;
	
	for(_uint i=0; i<5; i++)
	{
		if (FAILED(m_pPool_Instance->Add_ProjectilePool(ENUM_CLASS(LEVEL::QUEEN), TEXT("Prototype_Projectile_FireBall"), TEXT("FireBall"), &GameObjectDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Queen::Ready_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC MapDesc = {};
	MapDesc.strMapFilePath = "../Bin/Resources/Map/QueenMap.dat";
	MapDesc.eLevel = LEVEL::QUEEN;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Map"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), strLayerTag, &MapDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Queen* CLevel_Queen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Queen* pInstance = new CLevel_Queen(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Queen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Queen::Free()
{
	__super::Free();

	Safe_Release(m_pPool_Instance);
}
