#include "EnginePch.h"
#include "GameObject_Manager.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"

CGameObject_Manager::CGameObject_Manager()
	: m_pGameInstance {CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

CComponent* CGameObject_Manager::Get_Component(_uint iLayerLevel, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex)
{
	CLayer* pLayer = Find_Layer(iLayerLevel, strLayerTag);

	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_Component(strComponentTag, iIndex);
}

HRESULT CGameObject_Manager::Initialize()
{
	m_pLayers = new LAYERS[ENUM_CLASS(LAYERTYPE::END)];

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_ToLayer(_uint iLayerIndex, const _wstring& strLayerTag, _uint iPrototypeLevel, const _wstring& strPrototypeTag, void* pArg)
{
	if (iLayerIndex >= ENUM_CLASS(LAYERTYPE::END))
		return E_FAIL;

	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevel, strPrototypeTag, pArg));

	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		
		m_pLayers[iLayerIndex].emplace(strLayerTag, pLayer);
	}

	return pLayer->Add_GameObject(pGameObject);
}

HRESULT CGameObject_Manager::Add_GameObject_ToLayer(_uint iLayerIndex, const _wstring& strLayerTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLayerIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();

		m_pLayers[iLayerIndex].emplace(strLayerTag, pLayer);
	}

	if (SUCCEEDED(pLayer->Add_GameObject(pGameObject)))
		Safe_AddRef(pGameObject);
	else
		return E_FAIL;

	return S_OK;
}



void CGameObject_Manager::Post_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(LAYERTYPE::END); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Post_Update(fTimeDelta);
		}
	}
}

void CGameObject_Manager::Priority_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(LAYERTYPE::END); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if(nullptr != Pair.second)
				Pair.second->Priority_Update(fTimeDelta);
		}
	}
}

void CGameObject_Manager::Update(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(LAYERTYPE::END); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Update(fTimeDelta);
		}
	}
}

void CGameObject_Manager::Late_Update(_float fTimeDelta)
{
	for (_uint i = 0; i < ENUM_CLASS(LAYERTYPE::END); i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (nullptr != Pair.second)
				Pair.second->Late_Update(fTimeDelta);
		}
	}
}

void CGameObject_Manager::Clear()
{
	for (auto& Pair : m_pLayers[ENUM_CLASS(LAYERTYPE::NONSTATIC)])
		Safe_Release(Pair.second);

	m_pLayers[ENUM_CLASS(LAYERTYPE::NONSTATIC)].clear();
}

CLayer* CGameObject_Manager::Find_Layer(_uint iLayerIndex, const _wstring& strLayerTag)
{
	if (iLayerIndex >= ENUM_CLASS(LAYERTYPE::END))
		return nullptr;

	auto iter = m_pLayers[iLayerIndex].find(strLayerTag);

	if (iter == m_pLayers[iLayerIndex].end())
		return nullptr;

	return iter->second;
}

CGameObject_Manager* CGameObject_Manager::Create()
{
	CGameObject_Manager* pInstance = new CGameObject_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CGameObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGameObject_Manager::Free()
{
	__super::Free();

	for (_uint i = 0; i < ENUM_CLASS(LAYERTYPE::END); i++)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}

	Safe_Release(m_pGameInstance);

	Safe_Delete_Array(m_pLayers);
}
