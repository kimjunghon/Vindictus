#include "ClientPch.h"
#include "Palette.h"
#include "PlayerInstance.h"
#include "Armor.h"
#include "Weapon.h"
#include "ColorPoint.h"

CPalette::CPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext}
    , m_pPlayerInstance { CPlayerInstance::GetInstance()}
{
    Safe_AddRef(m_pPlayerInstance);
}

CPalette::CPalette(const CPalette& Prototype)
    : CUIObject { Prototype }
    , m_iPaletteWidth { Prototype.m_iPaletteWidth }
    , m_iPaletteHeight { Prototype.m_iPaletteWidth }
    ,m_pPlayerInstance { Prototype.m_pPlayerInstance }
{
    Safe_AddRef(m_pPlayerInstance);
}

HRESULT CPalette::Initialize_Prototype()
{
    m_iPaletteWidth = 256;
    m_iPaletteHeight = 256;

    return S_OK;
}

HRESULT CPalette::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_Palette()))
        return E_FAIL;

    if (FAILED(Ready_ColorPoint()))
        return E_FAIL;

    m_pPixels = new _uint[m_iPaletteHeight * m_iPaletteWidth];

    return S_OK;
}

void CPalette::Priority_Update(_float fTimeDelta)
{
}

void CPalette::Update(_float fTimeDelta)
{
    if (m_pDyeingWeapon || m_pDyeingArmor)
    {
        if (IsPick(g_hWnd))
        {
            if (false == m_IsMouseOn)
            {
                EVENT_MOUSE_CHANGE Event = {};
                Event.iMouseIndex = 1;
                m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
                m_IsMouseOn = true;
            }
        }
        else
        {
            if (m_IsMouseOn)
            {
                EVENT_MOUSE_CHANGE Event = {};
                Event.iMouseIndex = 0;
                m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);
                m_IsMouseOn = false;
            }
        }
    }
}

void CPalette::Late_Update(_float fTimeDelta)
{
    if(m_pDyeingWeapon || m_pDyeingArmor)
    {
        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
            return;

        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, m_pSelectColorPoint)))
            return;
    }
}

HRESULT CPalette::Render()
{
    __super::Begin();

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CPalette::Set_DyeingItem(pair<ITEM_TYPE, _uint>* pItemType)
{
    if (nullptr == pItemType)
        return;

    m_pDyeingDatas = nullptr;
    m_pDyeingWeapon = nullptr;
    m_pDyeingArmor = nullptr;

    if (pItemType->first == ITEM_TYPE::WEAPON)
    {
        m_pDyeingWeapon = m_pPlayerInstance->UpdatePlayerEquipWeapon(pItemType->second);
       if(nullptr != m_pDyeingWeapon)
           m_pDyeingDatas = m_pDyeingWeapon->Get_DyeingDatas();
    }
    else
    {
        m_pDyeingArmor = m_pPlayerInstance->UpdatePlayerEquipArmor(pItemType->second);
        if(nullptr != m_pDyeingArmor)
            m_pDyeingDatas = m_pDyeingArmor->Get_DyeingDatas();
    }

    Change_DyeingPart(0);
}

void CPalette::Change_DyeingPart(_uint iPartIndex)
{
    if (nullptr == m_pDyeingDatas ||
        iPartIndex >= ENUM_CLASS(DYEING_PART::END))
        return;

    m_iSelectPartIndex = iPartIndex;

    DYEING_MATERIAL eMaterial = (*m_pDyeingDatas)[iPartIndex].first;
    
    switch (eMaterial)
    {
    case DYEING_MATERIAL::CLOTH:
        m_iDyeMaterial = ENUM_CLASS(DYEING_MATERIAL::CLOTH);
        break;
    case DYEING_MATERIAL::LEATHER:
        m_iDyeMaterial = ENUM_CLASS(DYEING_MATERIAL::LEATHER);
        break;
    case DYEING_MATERIAL::WEAPON_METAL:
        m_iDyeMaterial = ENUM_CLASS(DYEING_MATERIAL::WEAPON_METAL);
        break;

    case DYEING_MATERIAL::ARMOR_METAL:
        m_iDyeMaterial = ENUM_CLASS(DYEING_MATERIAL::WEAPON_METAL); // ARMOR_METAL 포맷이 다름
        break;
    }

    m_pSelectColorPoint->Set_Color((*m_pDyeingDatas)[iPartIndex].second);

    m_pTextureCom->Copy_Resource(m_iDyeMaterial, m_pPaletteSample);
    Save_PaletteData();
}

void CPalette::Clear()
{
    m_pDyeingWeapon = nullptr;
    m_pDyeingArmor = nullptr;
    m_pDyeingDatas = nullptr;
}

void CPalette::Dyeing()
{
    if (nullptr == m_pDyeingWeapon && nullptr == m_pDyeingArmor)
        return;

    if (m_pDyeingWeapon)
        m_pDyeingWeapon->Dyeing(m_iSelectPartIndex, m_vDyeColor);
    else if (m_pDyeingArmor)
        m_pDyeingArmor->Dyeing(m_iSelectPartIndex, m_vDyeColor);

}

HRESULT CPalette::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_DyeingPalette"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CPalette::Ready_Palette()
{
    D3D11_TEXTURE2D_DESC TextureDesc = {};
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = m_iPaletteWidth;
    TextureDesc.Height = m_iPaletteHeight;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPaletteSample)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPalette::Ready_ColorPoint()
{
    CUIObject::UIOBJECT_DESC Color_Desc = {};
    Color_Desc.fX = m_fX;
    Color_Desc.fY = m_fY;
    Color_Desc.fSizeX = 60.f;
    Color_Desc.fSizeY = 60.f;
    Color_Desc.fOffsetX = -191.f;
    Color_Desc.fOffsetY = -120.f - m_fOffsetY;
    Color_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);

    m_pColorPoint = static_cast<CColorPoint*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_ColorPoint"), &Color_Desc));
    if (nullptr == m_pColorPoint)
        return E_FAIL; 

    Color_Desc.fSizeX = 32.f;
    Color_Desc.fSizeY = 32.f;
    Color_Desc.fOffsetX = 100.f;
    Color_Desc.fOffsetY = 135.f - m_fOffsetY;
    m_pSelectColorPoint = static_cast<CColorPoint*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_ColorPoint"), &Color_Desc));
    if (nullptr == m_pSelectColorPoint)
        return E_FAIL;
    return S_OK;
}

HRESULT CPalette::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iDyeMaterial)))
        return E_FAIL;

    return S_OK;
}

_bool CPalette::IsPick(HWND hWnd)
{
    _float fX = m_fX;
    _float fY = m_fY;

    RECT	rcRect = { LONG(fX - (m_fSizeX * 0.5f)), LONG(fY - (m_fSizeY * 0.5f)), LONG(fX + (m_fSizeX * 0.5f)), LONG(fY + (m_fSizeY * 0.5f)) };

    POINT	ptMouse = m_pGameInstance->Get_MousePoint();

    if (PtInRect(&rcRect, ptMouse))
    {
        _uint iX = ptMouse.x - rcRect.left;
        _uint iY = ptMouse.y - rcRect.top;

        _float3 vColor = PaletteColor(iX, iY);

        m_pColorPoint->Set_Color(vColor);

        if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, m_pColorPoint)))
            MSG_BOX(TEXT("Failed Add_RenderGroup : ColorPoint"));
        
        
        if (m_pGameInstance->Get_MouseState(MOUSEKEYSTATE::LB))
        {
            m_pSelectColorPoint->Set_Color(vColor);
            m_vDyeColor = vColor;
        }

        return true;
    }

    return false;
}

void CPalette::Save_PaletteData()
{
    D3D11_MAPPED_SUBRESOURCE	SubResource{};

    m_pDeviceContext->Map(m_pPaletteSample, 0, D3D11_MAP_READ, 0, &SubResource);

    memcpy(m_pPixels, SubResource.pData, sizeof(_uint) * (m_iPaletteHeight * m_iPaletteWidth));

    m_pDeviceContext->Unmap(m_pPaletteSample, 0);
}

_float3 CPalette::PaletteColor(_uint iX, _uint iY)
{
    _uint iIndex = iX + (iY * m_iPaletteWidth);

    _uint iPixelData = m_pPixels[iIndex];

    _float3 vColor = {};

    vColor.x = ((iPixelData & 0x00ff0000)>>16) / 255.f;
    vColor.y = ((iPixelData & 0x0000ff00)>>8) / 255.f;
    vColor.z = ((iPixelData & 0x000000ff)) / 255.f;

    return vColor;
}

CPalette* CPalette::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CPalette* pInstance = new CPalette(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CPalette"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPalette::Clone(void* pArg)
{
    CPalette* pInstance = new CPalette(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CPalette"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPalette::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pPixels);

    Safe_Release(m_pPlayerInstance);
    Safe_Release(m_pPaletteSample);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColorPoint);
    Safe_Release(m_pSelectColorPoint);
    
    m_pDyeingWeapon = nullptr;
    m_pDyeingArmor = nullptr;
    m_pDyeingDatas = nullptr;
}
