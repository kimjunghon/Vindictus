#include "EnginePch.h"
#include "UI_Slot.h"
#include "GameInstance.h"

CUI_Slot::CUI_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CUI_Slot::CUI_Slot(const CUI_Slot& Prototype)
	: CUIObject { Prototype }
	, m_iNumSlots { Prototype.m_iNumSlots }
{
}

HRESULT CUI_Slot::Initialize_Prototype(_uint iNumSlots)
{
	m_iNumSlots = iNumSlots;

	return S_OK;
}

HRESULT CUI_Slot::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_Children.resize(m_iNumSlots, nullptr);

	return S_OK;
}

void CUI_Slot::Priority_Update(_float fTimeDelta)
{
}

void CUI_Slot::Update(_float fTimeDelta)
{
}

void CUI_Slot::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Slot::Render()
{
	return S_OK;
}

void CUI_Slot::Set_Position(_float fX, _float fY)
{
	m_fX = fX + m_fOffsetX;
	m_fY = fY + m_fOffsetY;

	for (auto& pChild : m_Children)
		pChild->Set_Position(m_fX, m_fY);
}

void CUI_Slot::Children_Priority_Update(_float fTimeDelta)
{
	for (auto& pChild : m_Children)
	{
		if(nullptr != pChild)
			pChild->Priority_Update(fTimeDelta);
	}
}

void CUI_Slot::Children_Update(_float fTimeDelta)
{
	for (auto& pChild : m_Children)
	{
		if (nullptr != pChild)
			pChild->Update(fTimeDelta);
	}
}

void CUI_Slot::Children_Late_Update(_float fTimeDelta)
{
	for (auto& pChild : m_Children)
	{
		if (nullptr != pChild)
			pChild->Late_Update(fTimeDelta);
	}
}

HRESULT CUI_Slot::Add_Child(_uint iChildIndex, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, UIOBJECT_DESC* UIChildDesc)
{
	if (iChildIndex >= m_iNumSlots)
		return E_FAIL;

	CUIObject* pUIObject = dynamic_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
		iPrototypeLevelIndex, strPrototypeTag, UIChildDesc));;
	if (nullptr == pUIObject)
		return E_FAIL;

	m_Children[iChildIndex] = pUIObject;

	return S_OK;
}

void CUI_Slot::Free()
{
	__super::Free();

	for (auto& pChild : m_Children)
		Safe_Release(pChild);

	m_Children.clear();
}
