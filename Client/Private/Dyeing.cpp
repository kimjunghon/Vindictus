#include "ClientPch.h"
#include "Dyeing.h"
#include "DyeingSlots.h"
#include "DyeingPalette.h"

CDyeing::CDyeing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
{
}

CDyeing::CDyeing(const CDyeing& Prototype)
	: CUI_Panel { Prototype }
	, m_fClickDelay{ Prototype.m_fClickDelay }
{
}

HRESULT CDyeing::Initialize_Prototype()
{
	m_fClickDelay = 0.2f;

	return S_OK;
}

HRESULT CDyeing::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CDyeing::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::DYEING))
	{
		if (false == m_IsOpen)
		{
			m_pDyeingSlots->Update_Slots();
			m_IsOpen = true;
		}

		__super::Children_Priority_Update(fTimeDelta);
	}
	else
	{
		if (m_IsOpen)
		{
			m_pDyeingSlots->Clear_Slots();
			m_pDyeingPalette->ClearPalette();
			m_IsOpen = false;
		}
	}

}

void CDyeing::Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::DYEING))
	{
		m_fCurrentClickDelay += fTimeDelta;

		if (m_fClickDelay <= m_fCurrentClickDelay)
		{			
			if (m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::LB))
			{
				m_fCurrentClickDelay = 0.f;

				m_pDyeingPalette->Set_DyeingItem(m_pDyeingSlots->IsPickSlot(g_hWnd));
			}
		}
		__super::Children_Update(fTimeDelta);
	}
}

void CDyeing::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ENUM_CLASS(GAMEPLAY_FLAG::DYEING))
		__super::Children_Late_Update(fTimeDelta);
}

HRESULT CDyeing::Render()
{
	return S_OK;
}

HRESULT CDyeing::Ready_Children()
{
	CUI_Panel::PANEL_DESC Panel_Desc = {};
	Panel_Desc.fX = m_fX;
	Panel_Desc.fY = m_fY;
	Panel_Desc.fSizeX = 260.f;
	Panel_Desc.fSizeY = 420.f;
	Panel_Desc.fOffsetX = 350.f;
	Panel_Desc.fOffsetY = -100.f;
	Panel_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Panel_Desc.StateDesc.iUIState = m_pUIState;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_DyeingSlots"), &Panel_Desc, reinterpret_cast<CUIObject**>(&m_pDyeingSlots))))
		return E_FAIL;


	Panel_Desc.fSizeX = 400.f;
	Panel_Desc.fSizeY = 420.f;
	Panel_Desc.fOffsetX = -300.f;
	Panel_Desc.fOffsetY = -100.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_DyeingPalette"), &Panel_Desc, reinterpret_cast<CUIObject**>(&m_pDyeingPalette))))
		return E_FAIL;


	return S_OK;
}

CDyeing* CDyeing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDyeing* pInstance = new CDyeing(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CDyeing"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDyeing::Clone(void* pArg)
{
	CDyeing* pInstance = new CDyeing(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CDyeing"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDyeing::Free()
{
	__super::Free();

	Safe_Release(m_pDyeingSlots);
	Safe_Release(m_pDyeingPalette);
}
