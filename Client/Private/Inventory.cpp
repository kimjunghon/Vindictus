#include "ClientPch.h"
#include "Inventory.h"
#include "Storage.h"
#include "Equipment.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CInventory::CInventory(const CInventory& Prototype)
	: CUI_Panel { Prototype }
	, m_fClickDelay{ Prototype.m_fClickDelay }
{
}

HRESULT CInventory::Initialize_Prototype()
{
	m_fClickDelay = 0.2f;

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
	{
		if (m_fClickDelay <= m_fCurrentClickDelay)
		{
			if(m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::LB))
			{
				if (m_pStorage->IsPick(g_hWnd) || m_pEquipment->IsPick(g_hWnd))
				{
					m_fCurrentClickDelay = 0.f;
				}
				else
				{
					EVENT_NONE_PICK Event = {};
					m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), Event);

					m_fCurrentClickDelay = 0.f;
				}
			}
			else if (m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::RB))
			{
				if (m_pStorage->IsEquip(g_hWnd) || m_pEquipment->IsUnEquip(g_hWnd))
				{
					m_fCurrentClickDelay = 0.f;
				}
				else
					m_fCurrentClickDelay = 0.f;
			}
		}
		else
			m_fCurrentClickDelay += fTimeDelta;

		__super::Children_Update(fTimeDelta);
	}

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

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Storage"), &Panel_Desc, reinterpret_cast<CUIObject**>(&m_pStorage))))
		return E_FAIL;

	Panel_Desc.fSizeX = 260.f;
	Panel_Desc.fSizeY = 420.f;
	Panel_Desc.fOffsetX = 205.f;
	Panel_Desc.fOffsetY = -140.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Equipment"), &Panel_Desc, reinterpret_cast<CUIObject**>(&m_pEquipment))))
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

	Safe_Release(m_pStorage);
	Safe_Release(m_pEquipment);
}