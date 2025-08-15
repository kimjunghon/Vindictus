#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CMonsterInstance;

class CSpawn_Manager final : public CBase
{
private:
	typedef struct tagRoomData {
		vector<vector<MONSTER_SPAWN_DATA>> SpawnDatas;
		_uint iCurrentSpawnIndex = { 0 };
	}ROOM_SPAWN_DATA;

	typedef unordered_map<_uint, ROOM_SPAWN_DATA>	ROOM_SPAWN_MAP;

private:
	CSpawn_Manager();
	virtual ~CSpawn_Manager() = default;

public:	
	HRESULT		Ready_SpawnDatas(const Value& RoomSpawnDatas);
	void		ClearSpawnDatas();
	HRESULT		BeginRoomSpawn(_uint iRoomIndex);
	HRESULT		WaveEnd();

private:
	CMonsterInstance*		m_pMonsterInstance = { nullptr };
	ROOM_SPAWN_MAP			m_RoomData;
	ROOM_SPAWN_DATA*		m_pCurrentRoomData;
	_uint					m_iMaxWave = {};

public:
	static CSpawn_Manager*	Create();
	virtual void			Free() override;
};

NS_END