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
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMap::Priority_Update(_float fTimeDelta)
{
}

void CMap::Update(_float fTimeDelta)
{
}

void CMap::Late_Update(_float fTimeDelta)
{
}

HRESULT CMap::Render()
{
	return S_OK;
}

void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);

	for (auto& pMapObject : m_MapObjects)
		Safe_Release(pMapObject);
	m_MapObjects.clear();
}
