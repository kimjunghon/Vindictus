#include "ClientPch.h"
#include "Palette.h"

CPalette::CPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext}
{
}

CPalette::CPalette(const CPalette& Prototype)
    : CUIObject { Prototype }
    , m_iPaletteWidth { Prototype.m_iPaletteWidth }
    , m_iPaletteHeight { Prototype.m_iPaletteWidth }
{
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

    return S_OK;
}

void CPalette::Priority_Update(_float fTimeDelta)
{
}

void CPalette::Update(_float fTimeDelta)
{

}

void CPalette::Late_Update(_float fTimeDelta)
{

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CPalette::Render()
{
  /*  __super::Begin();

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();*/

    return S_OK;
}

HRESULT CPalette::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_Palette"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pPaletteSample)))
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

//    if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iDyeMaterial)))
//        return E_FAIL;

    return S_OK;
}

_float CPalette::Noise(_float2 vUV)
{
    _float2 vI = _float2(floor(vUV.x), floor(vUV.y));
    _float2 vF = _float2(fmod(vUV.x, 1.f), fmod(vUV.y, 1.f));

    _float fA = m_pGameInstance->Rand(0.f, 1.f);


    return _float();
}

_float CPalette::FBM(_float2 vUV)
{
    return _float();
}

_float3 CPalette::HSV_To_RGB(_float3 vHSV)
{
    _float3 vRGB = {};

    if (vHSV.y <= 0.f)
    {
        vRGB = _float3(vHSV.z, vHSV.z, vHSV.z);
        return vRGB;
    }

    _float fH = vHSV.x;
    _uint iHueSegment = static_cast<_uint>(fH);


    _float fSegment = fH - static_cast<_float>(iHueSegment);
    _float fMinValue = vHSV.z * (1.f - vHSV.y);
    _float fMixDown = vHSV.z * (1.f - (fSegment * vHSV.y));
    _float fMixUp = vHSV.z * (1.f - (vHSV.y * (1.f - fSegment)));

    switch (iHueSegment)
    {
    case 0:
        vRGB = _float3(vHSV.z, fMixUp, fMinValue);
        break;

    case 1:
        vRGB = _float3(fMixDown, vHSV.z, fMinValue);
        break;

    case 2:
        vRGB = _float3(fMinValue, vHSV.z, fMixUp);
        break;

    case 3:
        vRGB = _float3(fMinValue, fMixDown, vHSV.z);
        break;

    case 4:
        vRGB = _float3(fMixUp, fMinValue, vHSV.z);
        break;

    case 5:
    default:
        vRGB = _float3(vHSV.z, fMinValue, fMixDown);
        break;

    }

    return vRGB;
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

    Safe_Release(m_pPaletteSample);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
}
