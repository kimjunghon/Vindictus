#include "ClientPch.h"
#include "Inventory.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CInventory::CInventory(const CInventory& Prototype)
	: CUI_Panel { Prototype }
{
}

HRESULT CInventory::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInventory::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CInventory::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::INVENTORY))
		__super::Children_Priority_Update(fTimeDelta);
}

void CInventory::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::INVENTORY))
		__super::Children_Update(fTimeDelta);
}

void CInventory::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::INVENTORY))
		__super::Children_Late_Update(fTimeDelta);
}

HRESULT CInventory::Render()
{
	return S_OK;
}

HRESULT CInventory::Ready_Children()
{
	CUI_Panel::PANEL_DESC Panel_Desc = {};

	Panel_Desc.fX = m_fX;
	Panel_Desc.fY = m_fY;
	Panel_Desc.fSizeX = 330.f;
	Panel_Desc.fSizeY = 500.f;
	Panel_Desc.fOffsetX = 500.f;
	Panel_Desc.fOffsetY = -100.f;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.StateDesc.iUIState = m_pUIState;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Storage"), &Panel_Desc)))
		return E_FAIL;

	return S_OK;
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CInventory* pInstance = new CInventory(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CInventory"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
	CInventory* pInstance = new CInventory(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CInventory"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory::Free()
{
	__super::Free();
}