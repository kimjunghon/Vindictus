#include "ClientPch.h"
#include "Map.h"
#include "MapObject.h"

CMap::CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject { pDevice, pDeviceContext }
{
}

CMap::CMap(const CMap& Prototype)
	: CGameObject { Prototype }
{
}

HRESULT CMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMap::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	MAP_DESC* pDesc = static_cast<MAP_DESC*>(pArg);

	if (FAILED(Ready_MapObjects(pDesc->strMapFilePath, pDesc->eLevel)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMap::Priority_Update(_float fTimeDelta)
{
	for (auto& pMapObject : m_MapObjects)
		pMapObject->Priority_Update(fTimeDelta);
}

void CMap::Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

	for (auto& pMapObject : m_MapObjects)
		pMapObject->Update(fTimeDelta);
}

void CMap::Late_Update(_float fTimeDelta)
{
	for (auto& pMapObject : m_MapObjects)
		pMapObject->Late_Update(fTimeDelta);

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;

	if (FAILED(m_pGameInstance->Add_DebugComponent(m_pNavigationCom)))
		return;

#endif
}

HRESULT CMap::Render()
{

	return S_OK;
}

HRESULT CMap::Ready_MapObjects(string strMapFilePath, LEVEL eLevel)
{


	ifstream File(strMapFilePath, ios::binary);
	if (!File.is_open())
	{
		MSG_BOX(TEXT("Failed File Open TownMap"));
		return E_FAIL;
	}

	size_t iNumObjects = {};
	File.read(reinterpret_cast<_char*>(&iNumObjects), sizeof(size_t));

	for (size_t i = 0; i < iNumObjects; i++)
	{
		size_t iNameLength = {};
		_char szName[MAX_PATH] = {};
		_float4x4 WorldMatrx = {};

		File.read(reinterpret_cast<_char*>(&iNameLength), sizeof(size_t));
		File.read(szName, sizeof(_char) * iNameLength);
		File.read(reinterpret_cast<_char*>(&WorldMatrx), sizeof(_float4x4));

		_char szPrototype[MAX_PATH] = "Prototype_Component_Model_";
		_char szPrototypeTag[MAX_PATH] = {};
		strcpy_s(szPrototypeTag, szPrototype);
		strcat_s(szPrototypeTag, szName);

		_tchar szWidePrototypeTag[MAX_PATH] = {};

		MultiByteToWideChar(CP_UTF8, 0, szPrototypeTag, static_cast<_int>(strlen(szPrototypeTag)), szWidePrototypeTag, MAX_PATH);

		CMapObject::MAP_OBJECT_DESC MapObjectDesc = {};
		MapObjectDesc.iModelLevel = ENUM_CLASS(eLevel);
		MapObjectDesc.strModelTag = szWidePrototypeTag;
		MapObjectDesc.WorldMatrix = WorldMatrx;

		CMapObject* pMapObject = static_cast<CMapObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc));
		if (nullptr == pMapObject)
			return E_FAIL;

		m_MapObjects.push_back(pMapObject);
	}

	return S_OK;
}

HRESULT CMap::Ready_Components()
{
	m_pNavigationCom = m_pGameInstance->Clone_CurrentNavigation(-1);
	if (nullptr == m_pNavigationCom)
		return E_FAIL;

	return S_OK;
}

CMap* CMap::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMap* pInstance = new CMap(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CMap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMap::Clone(void* pArg)
{
	CMap* pInstance = new CMap(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CMap"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	for (auto& pMapObject : m_MapObjects)
		Safe_Release(pMapObject);
	m_MapObjects.clear();
}
