#include "ClientPch.h"
#include "Level_Town.h"

#include "Map.h"
#include "Camera_Free.h"
#include "PlayerPawn.h"
#include "Weapon.h"
#include "Armor.h"

CLevel_Town::CLevel_Town(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel{ pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
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

HRESULT CLevel_Town::Ready_DefaultWeapon()
{
	CWeapon::WEAPON_DESC WeaponDesc = {};
	WeaponDesc.iWeaponModelPrototypeLevelIndex = ENUM_CLASS(LEVEL::TOWN);
	WeaponDesc.strWeaponModelPrototypeTag = TEXT("Prototype_Component_Model_BastardSword");
	WeaponDesc.pPawnMatrix = nullptr;
	WeaponDesc.WeaponInfo = { TEXT("BastardSword"), 50.f, 0.f };
	WeaponDesc.vRotationQuaternion = XMQuaternionRotationRollPitchYaw(0.f, 0.f, XMConvertToRadians(90.f));
	WeaponDesc.eWeaponType = WEAPON_TYPE::SWORD;

	CGameObject* pBastardSword = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Weapon"), &WeaponDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::WEAPON, pBastardSword);


	WeaponDesc.strWeaponModelPrototypeTag = TEXT("Prototype_Component_Model_RoundShield");
	WeaponDesc.WeaponInfo = { TEXT("RoundShield"), 0.f, 20.f };
	WeaponDesc.vRotationQuaternion = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(60.f), XMConvertToRadians(90.f), XMConvertToRadians(180.f));
	WeaponDesc.eWeaponType = WEAPON_TYPE::SHILED;

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
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Upper"), 10.f, 5.f, 30.f };

	CGameObject* pLightMale_Upper = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Upper);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Lower");
	ArmorDesc.eArmorType = ARMOR_TYPE::LOWER;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Lower"), 10.f, 5.f, 30.f };

	CGameObject* pLightMale_Lower = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Lower);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Head");
	ArmorDesc.eArmorType = ARMOR_TYPE::HEAD;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Head"), 10.f, 5.f, 30.f };

	CGameObject* pLightMale_Head = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Head);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Hand");
	ArmorDesc.eArmorType = ARMOR_TYPE::HAND;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Hand"), 10.f, 5.f, 30.f };

	CGameObject* pLightMale_Hand = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Hand);

	ArmorDesc.strArmorModelPrototypeTag = TEXT("Prototype_Component_Model_LightMale_Foot");
	ArmorDesc.eArmorType = ARMOR_TYPE::FOOT;
	ArmorDesc.pPawnMatrix = nullptr;
	ArmorDesc.ArmorInfo = { TEXT("LightMale_Foot"), 10.f, 5.f, 30.f };

	CGameObject* pLightMale_Foot = static_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_Armor"), &ArmorDesc));
	m_pPlayerInstance->Add_Item(ITEM_TYPE::ARMOR, pLightMale_Foot);

	return S_OK;
}

HRESULT CLevel_Town::Ready_Map(const _wstring& strLayerTag)
{
	CMap::MAP_DESC MapDesc = {};
	MapDesc.strMapFilePath = "../Bin/Resources/Map/Town.dat";

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
}
