#include "ClientPch.h"
#include "Level_Town.h"

#include "Map.h"
#include "Camera_Free.h"
#include "PlayerPawn.h"
#include "Puppy.h"
#include "Cat.h"
#include "Weapon.h"
#include "Armor.h"
#include "Effect_Distortion.h"

CLevel_Town::CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
	, m_pPool_Instance	{ CPool_Instance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CLevel_Town::Initialize()
{
	if (FAILED(Ready_Light()))
		return E_FAIL;

	if (FAILED(Ready_GameObjectToJson()))
		return E_FAIL;
	
	if (FAILED(Ready_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_DefaultWeapon()))
		return E_FAIL;

	if (FAILED(Ready_DefaultArmor()))
		return E_FAIL;

	if (FAILED(Ready_Effect()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Town::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_RETURN))
	{
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::QUEEN);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
	}
}

HRESULT CLevel_Town::Render()
{
	return S_OK;
}

HRESULT CLevel_Town::Ready_Light()
{
	LIGHT_DESC			LightDesc = {};

	LightDesc.eType = LIGHT_DESC::TYPE::DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(TEXT("DIRECTONAL"), LightDesc)))
		return E_FAIL;

	SHADOW_LIGHT_DESC	ShadowLightDesc = {};
	ShadowLightDesc.vDirection = _float4(-1.f, -1.f, 0.5f, 0.f);
	ShadowLightDesc.fDistance = 2500.f;
	ShadowLightDesc.fFar = 5000.f;
	ShadowLightDesc.fNear = 1500.f;

	if (FAILED(m_pGameInstance->Update_ShadowLight(ShadowLightDesc)))
		return E_FAIL;

	m_pGameInstance->Update_ShadowLight(XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Town::Ready_GameObjectToJson()
{
	ifstream File("../Bin/Resources/Map/TownBat.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed TownBat Open"));
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

	if (Doc.HasMember("NPC") && Doc["NPC"].IsArray())
	{
		const Value& NPC = Doc["NPC"];

		if (FAILED(Ready_NPC(NPC)))
			return E_FAIL;
	}



	File.close();

	return S_OK;
}

HRESULT CLevel_Town::Ready_Player(const Value& Player)
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

HRESULT CLevel_Town::Ready_NPC(const Value& NPC_Data)
{
	for (auto& NPC : NPC_Data.GetArray())
	{
		string strName = {};
		if (NPC.HasMember("Name") && NPC["Name"].IsString())
			strName = NPC["Name"].GetString();
		
		_uint iCellIndex = {};
		if (NPC.HasMember("CellIndex") && NPC["CellIndex"].IsInt())
			iCellIndex = NPC["CellIndex"].GetInt();

		_float3 vPosition = {};
		if (NPC.HasMember("PositionX") && NPC["PositionX"].IsFloat()
			&& NPC.HasMember("PositionY") && NPC["PositionY"].IsFloat()
			&& NPC.HasMember("PositionZ") && NPC["PositionZ"].IsFloat())
		{
			vPosition = _float3(NPC["PositionX"].GetFloat(), NPC["PositionY"].GetFloat(), NPC["PositionZ"].GetFloat());
		}

		if (!strcmp(strName.c_str(), "Puppy"))
		{
			_uint iType = {};
			if (NPC.HasMember("Type") && NPC["Type"].IsInt())
				iType = NPC["Type"].GetInt();

			CPuppy::PUPPY_DESC PuppyDesc = {};
			PuppyDesc.iCellIndex = iCellIndex;
			PuppyDesc.vPosition = vPosition;
			PuppyDesc.iPuppyTypeIndex = iType;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Puppy"),
				ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_NPC"), &PuppyDesc)))
				return E_FAIL;
		}
		else if (!strcmp(strName.c_str(), "Cat"))
		{
			CCat::CAT_DESC CatDesc = {};
			CatDesc.iCellIndex = iCellIndex;
			CatDesc.vPosition = vPosition;

			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_Cat"),
				ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_NPC"), &CatDesc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Town::Ready_Effect()
{
	ifstream File("../Bin/Resources/EffectData/LoadFile/TownEffectPool.json");
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed TownEffectPool Open"));
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
				DistortionDesc.iLevel = ENUM_CLASS(LEVEL::TOWN);
				DistortionDesc.iPassIndex = iPassIndex;

				_bool IsMask = {};

				if (Effect.HasMember("IsMask") && Effect["IsMask"].IsBool())
					IsMask = Effect["IsMask"].GetBool();

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

				DistortionDesc.Is_Masking = IsMask;

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::TOWN), EffectName, strEffectTag, &DistortionDesc);

			}
			else
			{
				CEffect::EFFECT_DESC EffectDesc = {};

				EffectDesc.strEffectName = EffectName;
				EffectDesc.iPassIndex = iPassIndex;
				EffectDesc.iLevel = ENUM_CLASS(LEVEL::TOWN);

				for (_uint i = 0; i < iNumPool; i++)
					m_pPool_Instance->Add_EffectToPool(ENUM_CLASS(LEVEL::TOWN), EffectName, strEffectTag, &EffectDesc);
			}
		}
	}

	return S_OK;
}

HRESULT CLevel_Town::Ready_DefaultWeapon()
{
	CWeapon::WEAPON_DESC WeaponDesc = {};
	WeaponDesc.iWeaponModelPrototypeLevelIndex = ENUM_CLASS(LEVEL::TOWN);
	WeaponDesc.strWeaponModelPrototypeTag = TEXT("Prototype_Component_Model_BastardSword");
	WeaponDesc.pPawnMatrix = nullptr;
	WeaponDesc.WeaponInfo = { TEXT("BastardSword"), 50.f, 0.f };
	WeaponDesc.vOffsetPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	WeaponDesc.vRotationQuaternion = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	WeaponDesc.eWeaponType = WEAPON_TYPE::SWORD;
	WeaponDesc.Materials[0] = DYEING_MATERIAL::WEAPON_METAL;
	WeaponDesc.Materials[1] = DYEING_MATERIAL::WEAPON_METAL;
	WeaponDesc.Materials[2] = DYEING_MATERIAL::LEATHER;

	CGameObject* pBastardSword = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon"), &WeaponDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::WEAPON, pBastardSword);

	WeaponDesc.strWeaponModelPrototypeTag = TEXT("Prototype_Component_Model_RoundShield");
	WeaponDesc.WeaponInfo = { TEXT("RoundShield"), 0.f, 20.f };
	WeaponDesc.vOffsetPosition = XMVectorSet(1.5f, 0.3f, 0.f, 1.f);
	WeaponDesc.vRotationQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(75.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
	WeaponDesc.eWeaponType = WEAPON_TYPE::SHILED;
	WeaponDesc.Materials[0] = DYEING_MATERIAL::WEAPON_METAL;
	WeaponDesc.Materials[1] = DYEING_MATERIAL::WEAPON_METAL;
	WeaponDesc.Materials[2] = DYEING_MATERIAL::WEAPON_METAL;

	CGameObject* pRoundShield = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon"), &WeaponDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::WEAPON, pRoundShield);

	return S_OK;
}

HRESULT CLevel_Town::Ready_DefaultArmor()
{
	CArmor::ARMOR_DESC ArmorDesc = {};
	ArmorDesc.iArmorModelPrototypeLevelIndex = ENUM_CLASS(LEVEL::TOWN);
	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Upper");
	ArmorDesc.eArmorType = ARMOR_TYPE::UPPER;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Upper"), 10.f, 5.f, 30.f, 30.f };
	ArmorDesc.Materials[0] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[1] = DYEING_MATERIAL::LEATHER;
	ArmorDesc.Materials[2] = DYEING_MATERIAL::ARMOR_METAL;

	CGameObject* pLightMale_Upper = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Upper);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Lower");
	ArmorDesc.eArmorType = ARMOR_TYPE::LOWER;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Lower"), 10.f, 5.f, 30.f, 30.f };
	ArmorDesc.Materials[0] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[1] = DYEING_MATERIAL::LEATHER;
	ArmorDesc.Materials[2] = DYEING_MATERIAL::ARMOR_METAL;

	CGameObject* pLightMale_Lower = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Lower);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Head");
	ArmorDesc.eArmorType = ARMOR_TYPE::HEAD;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Head"), 10.f, 5.f, 30.f, 30.f };
	ArmorDesc.Materials[0] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[1] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[2] = DYEING_MATERIAL::ARMOR_METAL;

	CGameObject* pLightMale_Head = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Head);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Hand");
	ArmorDesc.eArmorType = ARMOR_TYPE::HAND;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Hand"), 10.f, 5.f, 30.f, 30.f };
	ArmorDesc.Materials[0] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[1] = DYEING_MATERIAL::LEATHER;
	ArmorDesc.Materials[2] = DYEING_MATERIAL::ARMOR_METAL;

	CGameObject* pLightMale_Hand = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Hand);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Foot");
	ArmorDesc.eArmorType = ARMOR_TYPE::FOOT;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Foot"), 10.f, 5.f, 30.f, 30.f };
	ArmorDesc.Materials[0] = DYEING_MATERIAL::ARMOR_METAL;
	ArmorDesc.Materials[1] = DYEING_MATERIAL::LEATHER;
	ArmorDesc.Materials[2] = DYEING_MATERIAL::ARMOR_METAL;

	CGameObject* pLightMale_Foot = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Foot);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC MapDesc = {};
	MapDesc.strMapFilePath = "../Bin/Resources/Map/Town.dat";
	MapDesc.eLevel = LEVEL::TOWN;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Map"),
		ENUM_CLASS(LAYER_TYPE::NONSTATIC), strLayerTag, &MapDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Town* CLevel_Town::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Town* pInstance = new CLevel_Town(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_Town"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Town::Free()
{
	__super::Free();
	
	Safe_Release(m_pPlayerInstance);
	Safe_Release(m_pPool_Instance);
}
