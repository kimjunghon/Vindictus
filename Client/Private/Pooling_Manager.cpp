#include "ClientPch.h"
#include "Pooling_Manager.h"
#include "MonsterInstance.h"
#include "Monster.h"

CPooling_Manager::CPooling_Manager()
	: m_pMonsterInstance { CMonsterInstance::GetInstance()}
	,m_pGameInstance { CGameInstance::GetInstance()}

{
	Safe_AddRef(m_pMonsterInstance);
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

		MONSTER_TYPE eMonsterType = m_pMonsterInstance->Get_MonsterType(strMonsterName);

		for (_uint i = 0; i < iNumMonster; i++)
		{
			if (FAILED(Add_MonsterToPool(eMonsterType)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CPooling_Manager::Clear_MonsterPool()
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
				m_pMonsterInstance->WaveEnd();

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

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(ENUM_CLASS(LAYERTYPE::NONSTATIC), TEXT("Layer_Monster"), pMonster)))
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

CMonster* CPooling_Manager::Clone_Monster(MONSTER_TYPE eMonsterType, void* pArg = nullptr)
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

	Clear_MonsterPool();

	Safe_Release(m_pMonsterInstance);
	Safe_Release(m_pGameInstance);

}
