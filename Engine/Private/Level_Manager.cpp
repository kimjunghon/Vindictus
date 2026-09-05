#include "EnginePch.h"
#include "Level_Manager.h"
#include "GameInstance.h"
#include "Level.h"

CLevel_Manager::CLevel_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}


void CLevel_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return;

	m_pCurrentLevel->Update(fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

HRESULT CLevel_Manager::Open_Level(_uint iLevelID, CLevel* pOpenLevel)
{
	if (nullptr == pOpenLevel)
		return E_FAIL;

	Safe_Release(m_pCurrentLevel);

	m_iCurrentLevelID = iLevelID;

	m_pCurrentLevel = pOpenLevel;

	return S_OK;
}

HRESULT CLevel_Manager::Clear_Resources()
{
	if (-1 == m_iCurrentLevelID)
		return S_OK;

	return m_pGameInstance->Clear_Resources(m_iCurrentLevelID);
}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);
}
