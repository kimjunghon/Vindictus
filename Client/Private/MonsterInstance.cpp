#include "ClientPch.h"
#include "MonsterInstance.h"
#include "Pooling_Manager.h"
#include "Spawn_Manager.h"

IMPLEMENT_SINGLETON(CMonsterInstance);

CMonsterInstance::CMonsterInstance()
{
}

HRESULT CMonsterInstance::Initialize()
{
	m_pPooling_Manager = CPooling_Manager::Create();
	if (nullptr == m_pPooling_Manager)
		return E_FAIL;

	m_pSpawn_Manager = CSpawn_Manager::Create();
	if (nullptr == m_pSpawn_Manager)
		return E_FAIL;


	if (FAILED(Ready_MonsterTypeMap()))
		return E_FAIL;

	return S_OK;
}

void CMonsterInstance::ClearLevel()
{
	m_pSpawn_Manager->ClearSpawnDatas();
	m_pPooling_Manager->Clear_MonsterPool();
}

HRESULT CMonsterInstance::Ready_Spawn_Data(const Value& RoomSpawnDatas)
{

	return m_pSpawn_Manager->Ready_SpawnDatas(RoomSpawnDatas);
}

HRESULT CMonsterInstance::BeginRoomSpawn(_uint iRoomIndex)
{
	return m_pSpawn_Manager->BeginRoomSpawn(iRoomIndex);
}

HRESULT CMonsterInstance::WaveEnd()
{
	return m_pSpawn_Manager->WaveEnd();
}

HRESULT CMonsterInstance::Ready_MonsterPool(const Value& MonsterPool)
{
	return m_pPooling_Manager->Ready_MonsterPool(MonsterPool);
}

HRESULT CMonsterInstance::Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData)
{
	return m_pPooling_Manager->Request_SpawnMonster(SpawnData);
}


HRESULT CMonsterInstance::Ready_MonsterTypeMap()
{
	m_MonsterTypeMap["Vampire_Basic"]	= MONSTER_TYPE::VAMPIRE_BASIC;
	m_MonsterTypeMap["Vampire_Elder"]	= MONSTER_TYPE::VAMPIRE_ELDER;
	m_MonsterTypeMap["Vampire_Royal"]	= MONSTER_TYPE::VAMPIRE_ROYAL;
	m_MonsterTypeMap["Queen"]			= MONSTER_TYPE::QUEEN;
	m_MonsterTypeMap["Glasgavelen"]		= MONSTER_TYPE::GLASGAVELEN;

	return S_OK;
}

void CMonsterInstance::Release_MonsterInstance()
{
	Release();

	Safe_Release(m_pPooling_Manager);
	Safe_Release(m_pSpawn_Manager);
}

void CMonsterInstance::Free()
{
	__super::Free();

}
