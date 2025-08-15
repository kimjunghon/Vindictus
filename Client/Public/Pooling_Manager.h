#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMonster;

class CPooling_Manager final : public CBase
{
private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

private:
	typedef unordered_map<string, queue<CMonster*>> MONSTER_POOL;
	typedef unordered_map<string, list<CMonster*>>  ACTIVE_MONSTERS;
	typedef unordered_map<_uint, vector<MONSTER_SPAWN_DATA>> MONSTER_SPAWN; // _uint == RoomIndex;

public:
	void		Clear_MonsterDatas();
	HRESULT		Add_Spawn_Data(MONSTER_SPAWN_DATA MonsterSpawnData);
	HRESULT		Request_SpawnMonster(_uint iRoomIndex);

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	MONSTER_POOL			m_Monster_Pool;
	MONSTER_SPAWN			m_Monster_Spawns;
	ACTIVE_MONSTERS			m_Active_Monsters;

public:
	static CPooling_Manager* Create();
	virtual void Free() override;
};

NS_END