#include "EnginePch.h"
#include "UI_Panel.h"
#include "GameInstance.h"

CUI_Panel::CUI_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext }
{
}

CUI_Panel::CUI_Panel(const CUI_Panel& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CUI_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Panel::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    PANEL_DESC* pDesc = static_cast<PANEL_DESC*>(pArg);
    m_pUIState = pDesc->StateDesc.iUIState;

    return S_OK;
}

void CUI_Panel::Priority_Update(_float fTimeDelta)
{
}

void CUI_Panel::Update(_float fTimeDelta)
{
}

void CUI_Panel::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Panel::Render()
{
    return S_OK;
}

void CUI_Panel::Set_Position(_float fX, _float fY)
{
    m_fX = fX + m_fOffsetX;
    m_fY = fY + m_fOffsetY;

    for (auto& pChild : m_Children)
        pChild->Set_Position(fX, fY);
}

void CUI_Panel::Set_Offset(_float fOffsetX, _float fOffsetY)
{
    m_fOffsetX = fOffsetX;
    m_fOffsetY = fOffsetY;
}

HRESULT CUI_Panel::Add_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* UIChildDesc)
{
    CUIObject* pUIObject = dynamic_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
        iPrototypeLevelIndex, strPrototypeTag, UIChildDesc));;
    if (nullptr == pUIObject)
        return E_FAIL;

    m_Children.push_back(pUIObject);

    return S_OK;
}

void CUI_Panel::Children_Priority_Update(_float fTimeDelta)
{
    if (m_Children.size() <= 0)
        return;

    for (auto& pChild : m_Children)
        pChild->Update(fTimeDelta);
}

void CUI_Panel::Children_Update(_float fTimeDelta)
{
    if (m_Children.size() <= 0)
        return;

    for (auto& pChild : m_Children)
        pChild->Update(fTimeDelta);
}

void CUI_Panel::Children_Late_Update(_float fTimeDelta)
{
    if (m_Children.size() <= 0)
        return;

    for (auto& pChild : m_Children)
        pChild->Late_Update(fTimeDelta);
    
}

void CUI_Panel::Free()
{
    __super::Free();

    for (auto& Child : m_Children)
        Safe_Release(Child);

    m_Children.clear();
}
