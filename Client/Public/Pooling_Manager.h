#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPool_Instance;
class CMonster;
class CProjectile;
class CEffect;
class CDamageFont;

class CPooling_Manager final : public CBase
{
private:
	typedef unordered_map<MONSTER_TYPE, queue<CMonster*>>	MONSTER_POOL;
	typedef list<pair<MONSTER_TYPE, CMonster*>>				ACTIVE_MONSTERS;
	
	//PROJ_TYPE
	typedef unordered_map<_wstring, queue<CProjectile*>>	PROJECTILE_POOL;
	typedef list<pair<_wstring, CProjectile*>>				ACTIVE_PROJECTILE;

	typedef unordered_map<_wstring, queue<CEffect*>>		EFFECT_POOL;
	typedef list<pair<_wstring, CEffect*>>					ACTIVE_EFFECT;

	typedef unordered_map<_wstring, queue<CDamageFont*>>		FONT_POOL;
	typedef list<pair<_wstring, CDamageFont*>>					ACTIVE_FONT;

private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;

public:
	HRESULT		Ready_MonsterPool(const Value& MonsterPool);
	HRESULT		Add_ProjectilePool(_uint iPrototypeLevelIndex, const _wstring& strProjectileTag, const _wstring& strProjectileName, void* pArg = nullptr);
	HRESULT		Add_EffectToPool(_uint iPrototypeLevelIndex, const _wstring& strEffectName, const _wstring& strEffectTag, void* pArg = nullptr);
	HRESULT		Add_DamageFont(_uint iPrototypeLevelIndex, const _wstring& strFontTag, const _wstring& strFontName, void* pArg = nullptr);
public:
	void		Clear_Pool();

	
	void		ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster);
	HRESULT		Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData);

	void		ReturnPool(const _wstring& strProjectileName, CProjectile* pProjectile);
	HRESULT		Request_SpawnProjectile(const _wstring& strProjectileName, void* pSpawnData = nullptr);

	void		ReturnPool(const _wstring& strEffect, CEffect* pEffect);
	HRESULT		Request_SpawnEffect(const _wstring& strEffect, void* pSpawnData = nullptr);

	void		ReturnPool(const _wstring& strFont, CDamageFont* pFont);
	HRESULT		Request_SpawnFont(const _wstring& strFont, void* pSpawnData = nullptr);

private:
	CPool_Instance*			m_pPool_Instance = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

	MONSTER_POOL			m_Monster_Pool;
	ACTIVE_MONSTERS			m_Active_Monsters;

	PROJECTILE_POOL			m_Projectile_Pool;
	ACTIVE_PROJECTILE		m_Active_Projectile;

	EFFECT_POOL				m_Effect_Pool;
	ACTIVE_EFFECT			m_Active_Effects;

	FONT_POOL				m_Font_Pool;
	ACTIVE_FONT				m_Active_Font;


private:
	HRESULT		Add_MonsterToPool(MONSTER_TYPE eMonsterType, void* pArg = nullptr);
	CMonster*	Clone_Monster(MONSTER_TYPE eMonsterType, void* pArg = nullptr);

public:
	static CPooling_Manager* Create();
	virtual void Free() override;
};

NS_END