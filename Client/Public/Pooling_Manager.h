#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CMonsterInstance;
class CMonster;

class CPooling_Manager final : public CBase
{
private:
	typedef unordered_map<MONSTER_TYPE, queue<CMonster*>>	MONSTER_POOL;
	typedef list<pair<MONSTER_TYPE, CMonster*>>				ACTIVE_MONSTERS;

private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

public:
	HRESULT		Ready_MonsterPool(const Value& MonsterPool);
	void		Clear_MonsterPool();
	void		ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster);
	HRESULT		Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData);

private:
	CMonsterInstance*		m_pMonsterInstance = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };
	MONSTER_POOL			m_Monster_Pool;
	ACTIVE_MONSTERS			m_Active_Monsters;

private:
	HRESULT		Add_MonsterToPool(MONSTER_TYPE eMonsterType, void* pArg = nullptr);
	CMonster*	Clone_Monster(MONSTER_TYPE eMonsterType, void* pArg = nullptr);

public:
	static CPooling_Manager* Create();
	virtual void Free() override;
};

NS_END