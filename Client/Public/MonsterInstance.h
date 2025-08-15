#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CPooling_Manager;
class CSpawn_Manager;

class CMonsterInstance final : public CBase
{
	DECLARE_SINGLETON(CMonsterInstance)

private:
	CMonsterInstance();
	virtual ~CMonsterInstance() = default;

public:
	MONSTER_TYPE	Get_MonsterType(string strMonsterName) { return m_MonsterTypeMap[strMonsterName]; }

public:
	HRESULT			Initialize();
	void			ClearLevel();

	HRESULT			Ready_Spawn_Data(const Value& RoomSpawnDatas);

	HRESULT			BeginRoomSpawn(_uint iRoomIndex);
	HRESULT			WaveEnd();

	HRESULT			Ready_MonsterPool(const Value& MonsterPool);
	HRESULT			Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData);
	
private:
	CSpawn_Manager*		m_pSpawn_Manager = { nullptr };
	CPooling_Manager*	m_pPooling_Manager = { nullptr };
	unordered_map<string, MONSTER_TYPE> m_MonsterTypeMap;

private:
	HRESULT		Ready_MonsterTypeMap();

public:
	void			Release_MonsterInstance();
	virtual void	Free() override;
};

NS_END