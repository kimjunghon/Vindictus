#include "ClientPch.h"
#include "Level_Glasgavelen.h"

#include "Map.h"
#include "PlayerPawn.h"
#include "Pool_Instance.h"

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

	if (FAILED(Ready_PoolingMonster()))
		return E_FAIL;

	if (FAILED(Ready_Map(TEXT("Layer_GameObject"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Glasgavelen::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyDown(DIK_F1))
	{
		m_pPool_Instance->BeginRoomSpawn(0);
	}

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
