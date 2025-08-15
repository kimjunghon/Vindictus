#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CPooling_Manager;

class CMonsterInstance final : public CBase
{
	DECLARE_SINGLETON(CMonsterInstance)

private:
	CMonsterInstance();
	virtual ~CMonsterInstance() = default;

public:
	HRESULT		Initialize();
	void		Clear_MonsterDatas();
	HRESULT		Add_Spawn_Data(MONSTER_SPAWN_DATA MonsterSpawnData);
	HRESULT		Request_SpawnMonster(_uint iRoomIndex);

private:
	CPooling_Manager*	m_pPooling_Manager = { nullptr };

public:
	void			Release_MonsterInstance();
	virtual void	Free() override;
};

NS_END