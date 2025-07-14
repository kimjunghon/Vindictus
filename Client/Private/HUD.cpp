#include "ClientPch.h"
#include "HUD.h"
#include "StateBar.h"

CHUD::CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{ pDevice, pDeviceContext }
{
}

CHUD::CHUD(const CHUD& Prototype)
	: CUI_Panel{ Prototype }
	, m_byVisibleType { Prototype.m_byVisibleType }
{
}

HRESULT CHUD::Initialize_Prototype()
{
	m_byVisibleType = ENUM_CLASS(GAMEPLAY_UI::DEFAULT);

	return S_OK;
}

HRESULT CHUD::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = true;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CHUD::Priority_Update(_float fTimeDelta)
{
	if(m_bVisible)
		__super::Children_Priority_Update(fTimeDelta);
}

void CHUD::Update(_float fTimeDelta)
{
	if (m_bVisible)
		__super::Children_Update(fTimeDelta);
}

void CHUD::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
		__super::Children_Late_Update(fTimeDelta);
}

HRESULT CHUD::Render()
{
	return S_OK;
}

HRESULT CHUD::Ready_Children()
{
	if (FAILED(Ready_PlayerStateBar()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHUD::Ready_PlayerStateBar()
{
	CStateBar::STATE_BAR_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 612.f;
	Children_Desc.fSizeY = 17.f;
	Children_Desc.fOffsetX = -200.f;
	Children_Desc.fOffsetY = -300.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	Children_Desc.eType = PROGRESS_TYPE::HP;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_StateBar"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeY = 12.f;
	Children_Desc.fOffsetY = -270.f;
	Children_Desc.eType = PROGRESS_TYPE::STAMINA;
	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_UIObject_StateBar"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

void CHUD::Event_UI_Change(const EVENT_UI_CHANGE& Event)
{
	if (Event.byVisibleType & m_byVisibleType)
		m_bVisible = true;
	else
		m_bVisible = false;
}

CHUD* CHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHUD* pInstance = new CHUD(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CHUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CHUD::Clone(void* pArg)
{
	CHUD* pInstance = new CHUD(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CHUD"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHUD::Free()
{
	__super::Free();
}