#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CPooling_Manager;
class CSpawn_Manager;
class CMonster;
class CEffect;
class CProjectile;
class CDamageFont;

class CPool_Instance final : public CBase
{
	DECLARE_SINGLETON(CPool_Instance)

private:
	CPool_Instance();
	virtual ~CPool_Instance() = default;

public:
	MONSTER_TYPE	Get_MonsterType(string strMonsterName) { return m_MonsterTypeMap[strMonsterName]; }

public:
	HRESULT			Initialize();
	void			ClearLevel();

	HRESULT			Ready_Spawn_Data(const Value& RoomSpawnDatas);

	void			ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster);
	HRESULT			BeginRoomSpawn(_uint iRoomIndex);
	HRESULT			WaveEnd();
	HRESULT			Ready_MonsterPool(const Value& MonsterPool);
	HRESULT			Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData);

	HRESULT			Add_ProjectilePool(_uint iPrototypeLevelIndex, const _wstring& strProjectileTag, const _wstring& strProjectileName, void* pArg = nullptr);
	void			ReturnPool(const _wstring& strProjectileName, CProjectile* pProjectile);
	HRESULT			Request_SpawnProjectile(const _wstring& strProjectileName, void* pSpawnData = nullptr);

	HRESULT			Add_EffectToPool(_uint iPrototypeLevelIndex, const _wstring& strEffectName, const _wstring& strEffectTag, void* pArg = nullptr);
	void			ReturnPool(const _wstring& strEffect, CEffect* pEffect);
	HRESULT			Request_SpawnEffect(const _wstring& strEffect, void* pSpawnData = nullptr);


	HRESULT			Add_DamageFont(_uint iPrototypeLevelIndex, const _wstring& strFontTag, const _wstring& strFontName, void* pArg = nullptr);
	void			ReturnPool(const _wstring& strFont, CDamageFont* pFont);
	HRESULT			Request_SpawnFont(const _wstring& strFont, void* pSpawnData = nullptr);
	
#ifdef _DEBUG
	HRESULT		SpawnRoom(_uint iRoomIndex, _uint iMonsterIndex);
#endif

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