#include "ClientPch.h"
#include "Pooling_Manager.h"
#include "Monster.h"

CPooling_Manager::CPooling_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CPooling_Manager::Clear_MonsterDatas()
{
}

HRESULT CPooling_Manager::Add_Spawn_Data(MONSTER_SPAWN_DATA MonsterSpawnData)
{
	return E_NOTIMPL;
}

HRESULT CPooling_Manager::Request_SpawnMonster(_uint iRoomIndex)
{
	return E_NOTIMPL;
}

CPooling_Manager* CPooling_Manager::Create()
{
	return nullptr;
}

void CPooling_Manager::Free()
{
	Safe_Release(m_pGameInstance);

}
