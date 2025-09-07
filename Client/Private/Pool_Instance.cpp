#include "ClientPch.h"
#include "Pool_Instance.h"
#include "Pooling_Manager.h"
#include "Spawn_Manager.h"
#include "Effect.h"
#include "Projectile.h"

IMPLEMENT_SINGLETON(CPool_Instance);

CPool_Instance::CPool_Instance()
{
}

HRESULT CPool_Instance::Initialize()
{
	m_pPooling_Manager = CPooling_Manager::Create();
	if (nullptr == m_pPooling_Manager)
		return E_FAIL;

	m_pSpawn_Manager = CSpawn_Manager::Create();
	if (nullptr == m_pSpawn_Manager)
		return E_FAIL;


	if (FAILED(Ready_MonsterTypeMap()))
		return E_FAIL;

	return S_OK;
}

void CPool_Instance::ClearLevel()
{
	m_pSpawn_Manager->ClearSpawnDatas();
	m_pPooling_Manager->Clear_Pool();
}

HRESULT CPool_Instance::Ready_Spawn_Data(const Value& RoomSpawnDatas)
{
	return m_pSpawn_Manager->Ready_SpawnDatas(RoomSpawnDatas);
}

void CPool_Instance::ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster)
{
	m_pPooling_Manager->ReturnPool(eMonsterType, pMonster);
}

HRESULT CPool_Instance::BeginRoomSpawn(_uint iRoomIndex)
{
	return m_pSpawn_Manager->BeginRoomSpawn(iRoomIndex);
}

HRESULT CPool_Instance::WaveEnd()
{
	return m_pSpawn_Manager->WaveEnd();
}

HRESULT CPool_Instance::Ready_MonsterPool(const Value& MonsterPool)
{
	return m_pPooling_Manager->Ready_MonsterPool(MonsterPool);
}

HRESULT CPool_Instance::Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData)
{
	return m_pPooling_Manager->Request_SpawnMonster(SpawnData);
}

HRESULT CPool_Instance::Add_ProjectilePool(_uint iPrototypeLevelIndex, const _wstring& strProjectileTag, const _wstring& strProjectileName, void* pArg)
{
	return m_pPooling_Manager->Add_ProjectilePool(iPrototypeLevelIndex, strProjectileTag, strProjectileName, pArg);
}

void CPool_Instance::ReturnPool(const _wstring& strProjectileName, CProjectile* pProjectile)
{
	m_pPooling_Manager->ReturnPool(strProjectileName, pProjectile);
}

HRESULT CPool_Instance::Request_SpawnProjectile(const _wstring& strProjectileName, void* pSpawnData)
{
	return m_pPooling_Manager->Request_SpawnProjectile(strProjectileName, pSpawnData);
}

HRESULT CPool_Instance::Add_EffectToPool(_uint iPrototypeLevelIndex, const _wstring& strEffectName, const _wstring& strEffectTag, void* pArg)
{
	return m_pPooling_Manager->Add_EffectToPool(iPrototypeLevelIndex, strEffectName, strEffectTag, pArg);
}

void CPool_Instance::ReturnPool(const _wstring& strEffect, CEffect* pEffect)
{
	m_pPooling_Manager->ReturnPool(strEffect, pEffect);
}

HRESULT CPool_Instance::Request_SpawnEffect(const _wstring& strEffect, void* pSpawnData)
{
	return m_pPooling_Manager->Request_SpawnEffect(strEffect, pSpawnData);
}


HRESULT CPool_Instance::Ready_MonsterTypeMap()
{
	m_MonsterTypeMap["Vampire_Basic"]	= MONSTER_TYPE::VAMPIRE_BASIC;
	m_MonsterTypeMap["Vampire_Elder"]	= MONSTER_TYPE::VAMPIRE_ELDER;
	m_MonsterTypeMap["Vampire_Royal"]	= MONSTER_TYPE::VAMPIRE_ROYAL;
	m_MonsterTypeMap["Queen"]			= MONSTER_TYPE::QUEEN;
	m_MonsterTypeMap["Glasgavelen"]		= MONSTER_TYPE::GLASGAVELEN;

	return S_OK;
}

void CPool_Instance::Release_MonsterInstance()
{
	Release();

	Safe_Release(m_pPooling_Manager);
	Safe_Release(m_pSpawn_Manager);
}

void CPool_Instance::Free()
{
	__super::Free();

}
