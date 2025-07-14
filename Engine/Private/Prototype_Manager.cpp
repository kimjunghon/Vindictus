#include "EnginePch.h"
#include "Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

#include "GameInstance.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint iNumLevels)
{
	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CPrototype_Manager::Add_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CBase* pPrototype)
{
	if (nullptr == pPrototype ||
		iPrototypeLevelIndex >= m_iNumLevels ||
		nullptr != Find_Prototype(iPrototypeLevelIndex, strPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iPrototypeLevelIndex].emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE ePrototype, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg)
{
	CBase* pPrototype = Find_Prototype(iPrototypeLevelIndex, strPrototypeTag);

	if (nullptr == pPrototype)
		return nullptr;

	CBase* pClone = { nullptr };

	switch (ePrototype)
	{
		case PROTOTYPE::GAMEOBJECT:
			pClone = dynamic_cast<CGameObject*>(pPrototype)->Clone(pArg);
			break;
		case PROTOTYPE::COMPONENT:
		{
			pClone = dynamic_cast<CComponent*>(pPrototype)->Clone(pArg);
			//CComponent* pComponent = dynamic_cast<CComponent*>(pPrototype);
			//if (pComponent->NeedCloned())
			//	pClone = pComponent->Clone(pArg);
			//else
			//{
			//	pClone = pComponent;
			//	Safe_AddRef(pClone);
			//}

			break;
		}
	}
	if (nullptr == pClone)
		return nullptr;

	return pClone;
}

void CPrototype_Manager::Clear(_uint iClearLevel)
{
	if (iClearLevel >= m_iNumLevels)
		return;

	for (auto& Pair : m_pPrototypes[iClearLevel])
		Safe_Release(Pair.second);

	m_pPrototypes[iClearLevel].clear();
}

CBase* CPrototype_Manager::Find_Prototype(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag)
{
	auto iter = m_pPrototypes[iPrototypeLevelIndex].find(strPrototypeTag);

	if (iter == m_pPrototypes[iPrototypeLevelIndex].end())
		return nullptr;

	return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint iNumLevel)
{
	CPrototype_Manager* pInstance = new CPrototype_Manager();

	if (FAILED(pInstance->Initialize(iNumLevel)))
	{
		MSG_BOX(TEXT("Failed to Created : CPrototype_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrototype_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}