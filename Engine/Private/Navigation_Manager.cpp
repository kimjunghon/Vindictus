#include "EnginePch.h"
#include "Navigation_Manager.h"
#include "Navigation.h"

CNavigation_Manager::CNavigation_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice }
	, m_pDeviceContext { pDeviceContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CNavigation_Manager::Add_Navigation(_uint iNavigationLevel, const _tchar* pNavigationFilePath)
{
	if (nullptr != Find_Navigation(iNavigationLevel))
		return E_FAIL;

	CNavigation* pNavigation = CNavigation::Create(m_pDevice, m_pDeviceContext, pNavigationFilePath);
	if (nullptr == pNavigation)
		return E_FAIL;

	m_Navigations.emplace(iNavigationLevel, pNavigation);

	return S_OK;
}

HRESULT CNavigation_Manager::Change_Navigation(_uint iNavigationLevel)
{
	if(nullptr != m_pCurrentNavigation)
		m_pCurrentNavigation = nullptr;
	
	m_pCurrentNavigation = Find_Navigation(iNavigationLevel);

	if (nullptr == m_pCurrentNavigation)
		return E_FAIL;

	return S_OK;
}

CNavigation* CNavigation_Manager::Clone_CurrentNavigation(_int iCellIndex)
{
	CNavigation::NAVIGATION_DESC NaviDesc = {};
	NaviDesc.iCurrentCellIndex = iCellIndex;

	CNavigation* pCloneNavi = static_cast<CNavigation*>(m_pCurrentNavigation->Clone(&NaviDesc));

	return pCloneNavi;
}

CNavigation* CNavigation_Manager::Find_Navigation(_uint iNavigationLevel)
{
	auto iter = m_Navigations.find(iNavigationLevel);

	if (iter == m_Navigations.end())
		return nullptr;

	return iter->second;
}

CNavigation_Manager* CNavigation_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return new CNavigation_Manager(pDevice, pDeviceContext);;
}

void CNavigation_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
