#include "ClientPch.h"
#include "Pooling_Manager.h"
#include "Pool_Instance.h"
#include "Monster.h"
#include "Projectile.h"
#include "Effect.h"
#include "DamageFont.h"

CPooling_Manager::CPooling_Manager()
	: m_pPool_Instance{ CPool_Instance::GetInstance()}
	,m_pGameInstance { CGameInstance::GetInstance()}

{
	Safe_AddRef(m_pPool_Instance);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPooling_Manager::Ready_MonsterPool(const Value& MonsterPool)
{
	if (false == MonsterPool.IsArray())
		return E_FAIL;

	for (auto& Monster : MonsterPool.GetArray())
	{
		_uint iNumMonster = {};
		if(Monster.HasMember("NumMonster") && Monster["NumMonster"].IsInt())
			iNumMonster = Monster["NumMonster"].GetInt();

		string strMonsterName = {};
		if (Monster.HasMember("Name") && Monster["Name"].IsString())
			strMonsterName = Monster["Name"].GetString();

		MONSTER_TYPE eMonsterType = m_pPool_Instance->Get_MonsterType(strMonsterName);

		for (_uint i = 0; i < iNumMonster; i++)
		{
			if (FAILED(Add_MonsterToPool(eMonsterType)))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPooling_Manager::Add_ProjectilePool(_uint iPrototypeLevelIndex, const _wstring& strProjectileTag, const _wstring& strProjectileName, void* pArg)
{
	CProjectile* pProjectile = static_cast<CProjectile*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strProjectileTag, pArg));
	if (nullptr == pProjectile)
		return E_FAIL;

	m_Projectile_Pool[strProjectileName].push(pProjectile);

	return S_OK;
}

HRESULT CPooling_Manager::Add_EffectToPool(_uint iPrototypeLevelIndex, const _wstring& strEffectName, const _wstring& strEffectTag, void* pArg)
{
	CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strEffectTag, pArg));

	if (nullptr == pEffect)
		return E_FAIL;

	m_Effect_Pool[strEffectName].push(pEffect);

	return S_OK;
}

HRESULT CPooling_Manager::Add_DamageFont(_uint iPrototypeLevelIndex, const _wstring& strFontTag, const _wstring& strFontName, void* pArg)
{
	CDamageFont* pFont = static_cast<CDamageFont*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strFontTag, pArg));
	if (nullptr == pFont)
		return E_FAIL;

	m_Font_Pool[strFontName].push(pFont);

	return S_OK;
}

void CPooling_Manager::Clear_Pool()
{
	for (auto& Pair : m_Monster_Pool)
	{
		while(false == Pair.second.empty())
		{
			Safe_Release(Pair.second.front());
			Pair.second.pop();
		}
	}
	m_Monster_Pool.clear();

	for (auto& Pair : m_Active_Monsters)
		Safe_Release(Pair.second);
	
	m_Active_Monsters.clear();

	for (auto& Pair : m_Projectile_Pool)
	{
		while (false == Pair.second.empty())
		{
			Safe_Release(Pair.second.front());
			Pair.second.pop();
		}
	}
	m_Projectile_Pool.clear();

	for (auto& Pair : m_Active_Projectile)
		Safe_Release(Pair.second);

	m_Active_Projectile.clear();

	for (auto& Pair : m_Effect_Pool)
	{
		while (false == Pair.second.empty())
		{
			Safe_Release(Pair.second.front());
			Pair.second.pop();
		}
	}
	m_Effect_Pool.clear();

	for (auto& Pair : m_Active_Effects)
		Safe_Release(Pair.second);

	m_Active_Effects.clear();

	for (auto& Pair : m_Font_Pool)
	{
		while (false == Pair.second.empty())
		{
			Safe_Release(Pair.second.front());
			Pair.second.pop();
		}
	}
	m_Font_Pool.clear();

	for (auto& Pair : m_Active_Font)
		Safe_Release(Pair.second);

	m_Active_Font.clear();
}

void CPooling_Manager::ReturnPool(MONSTER_TYPE eMonsterType, CMonster* pMonster)
{
	for (auto iter = m_Active_Monsters.begin(); iter != m_Active_Monsters.end();)
	{
		if ((iter->second) == pMonster)
		{
			m_Monster_Pool[eMonsterType].push(iter->second);
			iter = m_Active_Monsters.erase(iter);

			if (m_Active_Monsters.empty())
				m_pPool_Instance->WaveEnd();

			break;
		}
		else
			iter++;
	}
}

HRESULT CPooling_Manager::Request_SpawnMonster(MONSTER_SPAWN_DATA SpawnData)
{
	MONSTER_TYPE eMonsterType = SpawnData.eMonsterType;
	
	if (m_Monster_Pool[eMonsterType].empty())
		return E_FAIL;

	CMonster* pMonster = m_Monster_Pool[eMonsterType].front();
	pMonster->Spawn(SpawnData);

	m_Monster_Pool[eMonsterType].pop();
	m_Active_Monsters.push_back(make_pair(eMonsterType, pMonster));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Monster"), pMonster)))
		return E_FAIL;

	return S_OK;
}


void CPooling_Manager::ReturnPool(const _wstring& strProjectileName, CProjectile* pProjectile)
{
	for (auto iter = m_Active_Projectile.begin(); iter != m_Active_Projectile.end();)
	{
		if ((iter->second) == pProjectile)
		{
			m_Projectile_Pool[strProjectileName].push(iter->second);
			iter = m_Active_Projectile.erase(iter);
			break;
		}
		else
			iter++;
	}
}

HRESULT CPooling_Manager::Request_SpawnProjectile(const _wstring& strProjectileName, void* pSpawnData)
{
	if (m_Projectile_Pool[strProjectileName].empty())
		return E_FAIL;

	CProjectile* pProjectile = m_Projectile_Pool[strProjectileName].front();
	pProjectile->Spawn(pSpawnData);

	m_Projectile_Pool[strProjectileName].pop();
	m_Active_Projectile.push_back(make_pair(strProjectileName, pProjectile));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Projectile"), pProjectile)))
		return E_FAIL;
	
	return S_OK;
}

void CPooling_Manager::ReturnPool(const _wstring& strEffect, CEffect* pEffect)
{
	for (auto iter = m_Active_Effects.begin(); iter != m_Active_Effects.end();)
	{
		if ((iter->second) == pEffect)
		{
			m_Effect_Pool[strEffect].push(iter->second);
			iter = m_Active_Effects.erase(iter);
			break;
		}
		else
			iter++;
	}
}

HRESULT CPooling_Manager::Request_SpawnEffect(const _wstring& strEffect, void* pSpawnData)
{
	if (m_Effect_Pool[strEffect].empty())
		return E_FAIL;

	CEffect* pEffect = m_Effect_Pool[strEffect].front();
	pEffect->Spawn(pSpawnData);

	m_Effect_Pool[strEffect].pop();
	m_Active_Effects.push_back(make_pair(strEffect, pEffect));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Effect"), pEffect)))
		return E_FAIL;

	return S_OK;
}

void CPooling_Manager::ReturnPool(const _wstring& strFont, CDamageFont* pFont)
{
	for (auto iter = m_Active_Font.begin(); iter != m_Active_Font.end();)
	{
		if ((iter->second) == pFont)
		{
			m_Font_Pool[strFont].push(iter->second);
			iter = m_Active_Font.erase(iter);
			break;
		}
		else
			iter++;
	}
}

HRESULT CPooling_Manager::Request_SpawnFont(const _wstring& strFont, void* pSpawnData)
{
	if (m_Font_Pool[strFont].empty())
		return E_FAIL;

	CDamageFont* pFont = m_Font_Pool[strFont].front();
	pFont->Spawn(pSpawnData);

	m_Font_Pool[strFont].pop();
	m_Active_Font.push_back(make_pair(strFont, pFont));

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LAYER_TYPE::NONSTATIC), TEXT("Layer_Font"), pFont)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPooling_Manager::Add_MonsterToPool(MONSTER_TYPE eMonsterType, void* pArg)
{
	CMonster* pMonster = Clone_Monster(eMonsterType, pArg);
	if (nullptr == pMonster)
		return E_FAIL;

	m_Monster_Pool[eMonsterType].push(pMonster);

	return S_OK;
}

CMonster* CPooling_Manager::Clone_Monster(MONSTER_TYPE eMonsterType, void* pArg)
{
	CMonster* pMonster = { nullptr };

	_wstring strMonsterTag = {};

	switch (eMonsterType)
	{
	case MONSTER_TYPE::VAMPIRE_BASIC:
		strMonsterTag = TEXT("Prototype_GameObject_Vampire_Basic");
		break;

	case MONSTER_TYPE::VAMPIRE_ELDER:
		strMonsterTag = TEXT("Prototype_GameObject_Vampire_Elder");
		break;

	case MONSTER_TYPE::VAMPIRE_ROYAL:
		strMonsterTag = TEXT("Prototype_GameObject_Vampire_Royal");
		break;

	case MONSTER_TYPE::QUEEN:
		strMonsterTag = TEXT("Prototype_GameObject_Queen");
		break;

	case MONSTER_TYPE::GLASGAVELEN:
		strMonsterTag = TEXT("Prototype_GameObject_Glasgavelen");
		break;

	}

	pMonster = static_cast<CMonster*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), strMonsterTag, pArg));

	return pMonster;
}

CPooling_Manager* CPooling_Manager::Create()
{
	return new CPooling_Manager();
}

void CPooling_Manager::Free()
{
	__super::Free();

	Clear_Pool();

	Safe_Release(m_pPool_Instance);
	Safe_Release(m_pGameInstance);

}
