#include "ClientPch.h"
#include "Town.h"
#include "MapObject.h"

CTown::CTown(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CMap { pDevice, pDeviceContext }
{
}

CTown::CTown(const CTown& Prototype)
    : CMap { Prototype }
{
}

HRESULT CTown::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTown::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_MapObjects()))
        return E_FAIL;

    return S_OK;
}

void CTown::Priority_Update(_float fTimeDelta)
{
    for (auto& pMapObject : m_MapObjects)
        pMapObject->Priority_Update(fTimeDelta);
}

void CTown::Update(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom->Get_WorldMatrix());

    for (auto& pMapObject : m_MapObjects)
        pMapObject->Update(fTimeDelta);
}

void CTown::Late_Update(_float fTimeDelta)
{
    for (auto& pMapObject : m_MapObjects)
        pMapObject->Late_Update(fTimeDelta);

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
		return;
}

HRESULT CTown::Render()
{
#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CTown::Ready_MapObjects()
{
	ifstream File("../Bin/Resources/Town.dat", ios::binary);
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
		MapObjectDesc.iModelLevel = ENUM_CLASS(LEVEL::TOWN);
		MapObjectDesc.strModelTag = szWidePrototypeTag;
		MapObjectDesc.WorldMatrix = WorldMatrx;

		CMapObject* pMapObject = static_cast<CMapObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc));
		if (nullptr == pMapObject)
			return E_FAIL;

		m_MapObjects.push_back(pMapObject);
	}

    return S_OK;
}

HRESULT CTown::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::TOWN), TEXT("Prototype_Component_Navigation_Town"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), nullptr)))
		return E_FAIL;

    return S_OK;
}

CTown* CTown::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTown* pInstance = new CTown(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CTown"));
		Safe_Release(pInstance);
	}
    return pInstance;
}

CGameObject* CTown::Clone(void* pArg)
{
	CTown* pInstance = new CTown(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CTown"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTown::Free()
{
	__super::Free();
}
