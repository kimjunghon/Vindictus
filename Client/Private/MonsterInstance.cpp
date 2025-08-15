#include "ClientPch.h"
#include "MonsterInstance.h"
#include "Pooling_Manager.h"

IMPLEMENT_SINGLETON(CMonsterInstance);

CMonsterInstance::CMonsterInstance()
{
}

HRESULT CMonsterInstance::Initialize()
{
	m_pPooling_Manager = CPooling_Manager::Create();
	if (nullptr == m_pPooling_Manager)
		return E_FAIL;

	return S_OK;
}

void CMonsterInstance::Clear_MonsterDatas()
{

}

HRESULT CMonsterInstance::Add_Spawn_Data(MONSTER_SPAWN_DATA MonsterSpawnData)
{
	return m_pPooling_Manager->Add_Spawn_Data(MonsterSpawnData);
}

HRESULT CMonsterInstance::Request_SpawnMonster(_uint iRoomIndex)
{
	return m_pPooling_Manager->Request_SpawnMonster(iRoomIndex);
}

void CMonsterInstance::Release_MonsterInstance()
{
	Release();

	Safe_Release(m_pPooling_Manager);
}

void CMonsterInstance::Free()
{
	__super::Free();

}
