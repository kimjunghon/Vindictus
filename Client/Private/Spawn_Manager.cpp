#include "ClientPch.h"
#include "Spawn_Manager.h"
#include "Pool_Instance.h"

CSpawn_Manager::CSpawn_Manager()
	: m_pPool_Instance{ CPool_Instance::GetInstance() }
{
	Safe_AddRef(m_pPool_Instance);
}

HRESULT CSpawn_Manager::Ready_SpawnDatas(const Value& RoomDatas)
{
	if (false == RoomDatas.IsArray())
		return E_FAIL;

	for (auto& RoomData : RoomDatas.GetArray())
	{
		_uint iRoomIndex = {};

		if (RoomData.HasMember("RoomIndex") && RoomData["RoomIndex"].IsInt())
			iRoomIndex = RoomData["RoomIndex"].GetInt();

		if (RoomData.HasMember("Waves") && RoomData["Waves"].IsArray())
		{
			const Value& Waves = RoomData["Waves"];

			ROOM_SPAWN_DATA RoomSpawnData = {};
			RoomSpawnData.iCurrentSpawnIndex = 0;

			for (auto& Wave : Waves.GetArray())
			{
				if(Wave.HasMember("Monsters") && Wave["Monsters"].IsArray())
				{
					const Value& SpawnDatas = Wave["Monsters"];

					vector<MONSTER_SPAWN_DATA> MonsterSpawnDatas;

					for (auto& SpawnData : SpawnDatas.GetArray())
					{
						MONSTER_SPAWN_DATA MonsterSpawnData = {};

						string strMonsterName = {};

						if (SpawnData.HasMember("Name") && SpawnData["Name"].IsString())
						{
							strMonsterName = SpawnData["Name"].GetString();
							MonsterSpawnData.eMonsterType = m_pPool_Instance->Get_MonsterType(strMonsterName);
						}

						if (SpawnData.HasMember("CellIndex") && SpawnData["CellIndex"].IsInt())
							MonsterSpawnData.iCellIndex = SpawnData["CellIndex"].GetInt();

						if (SpawnData.HasMember("PositionX") && SpawnData["PositionX"].IsFloat()
							&& SpawnData.HasMember("PositionY") && SpawnData["PositionY"].IsFloat()
							&& SpawnData.HasMember("PositionZ") && SpawnData["PositionZ"].IsFloat())
						{
							MonsterSpawnData.vPosition = _float3(SpawnData["PositionX"].GetFloat(), SpawnData["PositionY"].GetFloat(), SpawnData["PositionZ"].GetFloat());
						}

						MonsterSpawnDatas.push_back(MonsterSpawnData);
					}
					
					RoomSpawnData.SpawnDatas.push_back(MonsterSpawnDatas);
				}
			}

			m_RoomData.emplace(iRoomIndex, RoomSpawnData);
		}
	}

	return S_OK;
}

void CSpawn_Manager::ClearSpawnDatas()
{
	m_RoomData.clear();

	m_pCurrentRoomData = nullptr;
}

HRESULT CSpawn_Manager::BeginRoomSpawn(_uint iRoomIndex)
{
	auto iter = m_RoomData.find(iRoomIndex);
	if (iter == m_RoomData.end())
		return E_FAIL;

	m_pCurrentRoomData = &m_RoomData[iRoomIndex];
	m_pCurrentRoomData->iCurrentSpawnIndex = 0;

	m_iMaxWave = static_cast<_uint>(m_pCurrentRoomData->SpawnDatas.size());

	//m_pPool_Instance->Request_SpawnMonster(m_pCurrentRoomData->SpawnDatas[3][0]);

	for (auto& SpawnData : m_pCurrentRoomData->SpawnDatas[m_pCurrentRoomData->iCurrentSpawnIndex])
		m_pPool_Instance->Request_SpawnMonster(SpawnData);
	
	return S_OK;
}
#ifdef _DEBUG
HRESULT CSpawn_Manager::SpawnRoom(_uint iRoomIndex, _uint iMonsterIndex)
{
	m_pCurrentRoomData = &m_RoomData[0];
	m_pCurrentRoomData->iCurrentSpawnIndex = 0;

	m_pPool_Instance->Request_SpawnMonster(m_pCurrentRoomData->SpawnDatas[iRoomIndex][iMonsterIndex]);

	return S_OK;
}
#endif
HRESULT CSpawn_Manager::WaveEnd()
{
	if (nullptr == m_pCurrentRoomData)
		return E_FAIL;

	m_pCurrentRoomData->iCurrentSpawnIndex++;

	if (m_pCurrentRoomData->iCurrentSpawnIndex >= m_iMaxWave)
	{
		// 문이 열리는 이벤트
		return S_OK;
	}

	if(m_pCurrentRoomData->SpawnDatas[m_pCurrentRoomData->iCurrentSpawnIndex].empty())
		return S_OK;

	for (auto& MonsterSpawn : m_pCurrentRoomData->SpawnDatas[m_pCurrentRoomData->iCurrentSpawnIndex])
		m_pPool_Instance->Request_SpawnMonster(MonsterSpawn);

	return S_OK;
}

CSpawn_Manager* CSpawn_Manager::Create()
{
	return new CSpawn_Manager();
}

void CSpawn_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pPool_Instance);
}
