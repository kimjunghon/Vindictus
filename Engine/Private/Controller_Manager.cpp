#include "Controller_Manager.h"
#include "Controller.h"

CController_Manager::CController_Manager()
{
}

HRESULT CController_Manager::MoveInput(_uint iChannelIndex, INPUT_MOVE_DESC* pOut)
{
	if (iChannelIndex >= ENUM_CLASS(CONTROLLER_CHANNEL::END) ||
		nullptr != m_pCurrentController[iChannelIndex])
		return E_FAIL;

	return m_pCurrentController[iChannelIndex]->MoveInput(pOut);
}

HRESULT CController_Manager::ActionInput(_uint iChannelIndex, INPUT_ACTION_DESC* pOut)
{
	if (iChannelIndex >= ENUM_CLASS(CONTROLLER_CHANNEL::END) ||
		nullptr != m_pCurrentController[iChannelIndex])
		return E_FAIL;

	return m_pCurrentController[iChannelIndex]->ActionInput(pOut);
}

HRESULT CController_Manager::CameraInput(_uint iChannelIndex, INPUT_CAMERA_DESC* pOut)
{
	if (iChannelIndex >= ENUM_CLASS(CONTROLLER_CHANNEL::END) ||
		nullptr != m_pCurrentController[iChannelIndex])
		return E_FAIL;

	return m_pCurrentController[iChannelIndex]->CameraInput(pOut);
}


HRESULT CController_Manager::Add_Controller_ToManager(const _wstring& strControllerTag, CController* pController)
{
	if (nullptr == pController &&
		nullptr != Find_Controller(strControllerTag))
		return E_FAIL;

	m_Controllers.emplace(strControllerTag, pController);
	Safe_AddRef(pController);

	return S_OK;
}

HRESULT CController_Manager::Change_Controller(_uint iChannelIndex, const _wstring& strControllerTag)
{
	CController* pController = Find_Controller(strControllerTag);
	if (nullptr == pController)
		return E_FAIL;

	m_pCurrentController[iChannelIndex] = pController;

	return S_OK;
}

HRESULT CController_Manager::Change_Controller(_uint iChannelIndex, CController* pNewController)
{
	if (nullptr == pNewController)
		return E_FAIL;

	m_pCurrentController[iChannelIndex] = pNewController;

	return S_OK;
}

CController* CController_Manager::Find_Controller(const _wstring& strControllerTag)
{
	auto Pair = m_Controllers.find(strControllerTag);
	if (Pair == m_Controllers.end())
		return nullptr;

	return Pair->second;
}

void CController_Manager::Clear()
{
	for (auto& Pair : m_Controllers)
		Safe_Release(Pair.second);

	m_Controllers.clear();
}

CController_Manager* CController_Manager::Create()
{
	return new CController_Manager();
}

void CController_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Controllers)
		Safe_Release(Pair.second);

	m_Controllers.clear();
}
