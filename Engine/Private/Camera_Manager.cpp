#include "EnginePch.h"
#include "Camera_Manager.h"
#include "GameInstance.h"
#include "Camera.h"

CCamera_Manager::CCamera_Manager()
	: m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

void CCamera_Manager::Update(_float fTimeDelta)
{
	if (nullptr == m_pCurrentCamera)
		return;

	m_pCurrentCamera->Update(fTimeDelta);
	m_pCurrentCamera->Update_PipeLines();
}

HRESULT CCamera_Manager::Add_CameraToManager(_uint iPrototypeIndex, const _wstring& strPrototypeTag, const _wstring& strCameraTag, CCamera** ppOut, void* pArg)
{
	if(nullptr != Find_Camera(strCameraTag))
		return E_FAIL;

	CCamera* pCamera = dynamic_cast<CCamera*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeIndex, strPrototypeTag, pArg));
	if (nullptr == pCamera)
		return E_FAIL;

	if(nullptr !=ppOut)
	{
		*ppOut = pCamera;
		Safe_AddRef(pCamera);
	}

	m_Cameras.emplace(strCameraTag, pCamera);
	
	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(const _wstring& strCameraTag, void* pArg)
{
	CCamera* pCamera = Find_Camera(strCameraTag);
	if (nullptr == pCamera)
		return E_FAIL;

	pCamera->Reset(pArg);

	m_pCurrentCamera = pCamera;

	return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(CCamera* pNewCamera)
{
	if (nullptr == pNewCamera)
		return E_FAIL;

	m_pCurrentCamera = pNewCamera;

	return S_OK;
}

CCamera* CCamera_Manager::Find_Camera(const _wstring& strCameraTag)
{
	auto Pair = m_Cameras.find(strCameraTag);
	if (Pair == m_Cameras.end())
		return nullptr;

	return Pair->second;
}

void CCamera_Manager::Clear()
{
	for (auto& Pair : m_Cameras)
		Safe_Release(Pair.second);

	m_Cameras.clear();

	m_pCurrentCamera = nullptr;
}

CCamera_Manager* CCamera_Manager::Create()
{
	return new CCamera_Manager();
}

void CCamera_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	for (auto& Pair : m_Cameras)
		Safe_Release(Pair.second);

	m_Cameras.clear();
}
