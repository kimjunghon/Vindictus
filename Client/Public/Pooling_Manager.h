#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPool_Instance;
class CMonster;
class CEffect;

class CPooling_Manager final : public CBase
{
private:
	typedef unordered_map<MONSTER_TYPE, queue<CMonster*>>	MONSTER_POOL;
	typedef list<pair<MONSTER_TYPE, CMonster*>>				ACTIVE_MONSTERS;
	
	//PROJ_TYPE
	typedef unordered_map<_uint, queue<CGameObject*>>	PROJECTILE_POOL;
	typedef list<pair<_uint, CGameObject*>>				ACTIVE_PROJECTILE;

	typedef unordered_map<_wstring, queue<CEffect*>>		EFFECT_POOL;
	typedef list<pair<_wstring, CEffect*>>				ACTIVE_EFFECT;

private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;


public:
	HRESULT		Ready_MonsterPool(const Value& MonsterPool);
	
	HRESULT		Add_EffectToPool(_uint iPrototypeLevelIndex, const _wstring& strEffectName, const _wstring& strEffectTag, void* pArg = nullptr);

public:
	void		Clear_Pool();

	
	void		ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster);
	HRESULT		Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData);


	void		ReturnPool(const _wstring& strEffect, CEffect* pEffect);
	HRESULT		Request_SpawnEffect(const _wstring& strEffect, void* pSpawnData = nullptr);

private:
	CPool_Instance*			m_pPool_Instance = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

	MONSTER_POOL			m_Monster_Pool;
	ACTIVE_MONSTERS			m_Active_Monsters;

	EFFECT_POOL				m_Effect_Pool;
	ACTIVE_EFFECT			m_Active_Effects;

private:
	HRESULT		Add_MonsterToPool(MONSTER_TYPE eMonsterType, void* pArg = nullptr);
	CMonster*	Clone_Monster(MONSTER_TYPE eMonsterType, void* pArg = nullptr);

public:
	static CPooling_Manager* Create();
	virtual void Free() override;
};

NS_END