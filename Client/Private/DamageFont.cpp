#include "ClientPch.h"
#include "DamageFont.h"

CDamageFont::CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUIObject { pDevice, pDeviceContext }
    , m_pPoolInstance { CPool_Instance::GetInstance()}
{
    Safe_AddRef(m_pPoolInstance);
}

CDamageFont::CDamageFont(const CDamageFont& Prototype)
    : CUIObject { Prototype }
    , m_MonsterFontDatas { Prototype.m_MonsterFontDatas }
    , m_PlayerFontDatas { Prototype.m_PlayerFontDatas}
    , m_pPoolInstance { Prototype.m_pPoolInstance }
    , m_fLifeTime { Prototype.m_fLifeTime }
{
    Safe_AddRef(m_pPoolInstance);
}

HRESULT CDamageFont::Initialize_Prototype(const _char* pFontDataFilePath)
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    if (FAILED(Ready_FontDatas(pFontDataFilePath)))
        return E_FAIL;

    m_fLifeTime = 1.f;

    return S_OK;
}

HRESULT CDamageFont::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDamageFont::Priority_Update(_float fTimeDelta)
{
}

void CDamageFont::Update(_float fTimeDelta)
{
    m_fCurrentLifeTime += fTimeDelta;

    if (m_fCurrentLifeTime >= m_fLifeTime)
    {
        m_IsActive = false;
        m_pPoolInstance->ReturnPool(TEXT("DamageFont"), this);
    }

}

void CDamageFont::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

HRESULT CDamageFont::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DAMAGE_FONT));

    m_pVIBufferCom->Bind_Resources();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CDamageFont::Spawn(void* pArg)
{
    m_IsActive = true;

    m_fCurrentLifeTime = 0.f;

    DAMAGE_DESC* pDesc = static_cast<DAMAGE_DESC*>(pArg);

    _vector vPosition = pDesc->vPosition;

    _matrix ViewMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW));
    _matrix ProjMatrix = XMLoadFloat4x4(m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ));
    
    vPosition = XMVector4Transform(vPosition, ViewMatrix);
    vPosition = XMVector4Transform(vPosition, ProjMatrix);
    
    vPosition = XMVectorScale(vPosition, 1 / XMVectorGetW(vPosition));

    m_fX = (XMVectorGetX(vPosition) * 0.5f + 0.5f) * g_iWinSizeX;
    m_fY = (XMVectorGetY(vPosition) * -0.5f + 0.5f) * g_iWinSizeY;
    
    m_iDamage = pDesc->iDamage;

    Update_Buffer(pDesc->eOwner);

    return S_OK;
}

HRESULT CDamageFont::Ready_Components()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Dynamic"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_DamageFont"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDamageFont::Ready_FontDatas(const _char* pFontDataFilePath)
{
    ifstream File(pFontDataFilePath);
    if (!File.is_open())
    {
        MSG_BOX(TEXT("Failed FontDataFile Open"));
        return E_FAIL;
    }

    IStreamWrapper FileWrap(File);

    Document Doc;
    Doc.ParseStream(FileWrap);

    if (Doc.HasParseError())
    {
        MSG_BOX(TEXT("Failed ParseStream"));
        return E_FAIL;
    }

    _uint iTextureSizeX = {};
    _uint iTextureSizeY = {};
    _uint iMonsterStartY = {};
    _uint iMonsterEndY = {};
    _uint iPlayerStartY = {};
    _uint iPlayerEndY = {};

    if (Doc.HasMember("TextureSizeX") && Doc["TextureSizeX"].IsInt())
        iTextureSizeX = Doc["TextureSizeX"].GetInt();

    if (Doc.HasMember("TextureSizeY") && Doc["TextureSizeY"].IsInt())
        iTextureSizeY = Doc["TextureSizeY"].GetInt();

    if (Doc.HasMember("MonsterFontStartY") && Doc["MonsterFontStartY"].IsInt())
        iMonsterStartY = Doc["MonsterFontStartY"].GetInt();

    if (Doc.HasMember("MonsterFontEndY") && Doc["MonsterFontEndY"].IsInt())
        iMonsterEndY = Doc["MonsterFontEndY"].GetInt();

    if (Doc.HasMember("PlayerFontStartY") && Doc["PlayerFontStartY"].IsInt())
        iPlayerStartY = Doc["PlayerFontStartY"].GetInt();

    if (Doc.HasMember("PlayerFontEndY") && Doc["PlayerFontEndY"].IsInt())
        iPlayerEndY = Doc["PlayerFontEndY"].GetInt();

    if (Doc.HasMember("Fonts") && Doc["Fonts"].IsArray())
    {
        const Value& Fonts = Doc["Fonts"];

        for (auto& Font : Fonts.GetArray())
        {
            string szFont = {};
            _uint iStartX = {};
            _uint iEndX = {};

            if (Font.HasMember("Font") && Font["Font"].IsString())
                szFont = Font["Font"].GetString();

            if (Font.HasMember("StartX") && Font["StartX"].IsInt())
                iStartX = Font["StartX"].GetInt();

            if (Font.HasMember("EndX") && Font["EndX"].IsInt())
                iEndX = Font["EndX"].GetInt();

            Add_MonsterData(iTextureSizeX, iTextureSizeY, iStartX, iEndX, iMonsterStartY, iMonsterEndY, szFont[0]);
            Add_PlayerData(iTextureSizeX, iTextureSizeY, iStartX, iEndX, iPlayerStartY, iPlayerEndY, szFont[0]);
        }
    }

    return S_OK;
}

void CDamageFont::Add_MonsterData(_uint iTextureSizeX, _uint iTextureSizeY, _uint iStartX, _uint iEndX, _uint iStartY, _uint iEndY, _char chFont)
{
    _float fSizeX = static_cast<_float>(iEndX - iStartX);
    _float fSizeY = static_cast<_float>(iEndY - iStartY);

    _float fStartU = static_cast<_float>(iStartX) / static_cast<_float>(iTextureSizeX);
    _float fStartV = static_cast<_float>(iStartY) / static_cast<_float>(iTextureSizeY);
    _float fEndU = static_cast<_float>(iEndX) / static_cast<_float>(iTextureSizeX);
    _float fEndV = static_cast<_float>(iEndY) / static_cast<_float>(iTextureSizeY);

    _float2 vSize = _float2(fSizeX, fSizeY);
    _float4 vUV = _float4(fStartU, fStartV, fEndU, fEndV);

    ATLAS_DATA Data = {};
    Data.vSize = vSize;
    Data.vUV = vUV;

    m_MonsterFontDatas.emplace(chFont, Data);
}

void CDamageFont::Add_PlayerData(_uint iTextureSizeX, _uint iTextureSizeY, _uint iStartX, _uint iEndX, _uint iStartY, _uint iEndY, _char chFont)
{
    _float fSizeX = static_cast<_float>(iEndX - iStartX);
    _float fSizeY = static_cast<_float>(iEndY - iStartY);

    _float fStartU = static_cast<_float>(iStartX) / static_cast<_float>(iTextureSizeX);
    _float fStartV = static_cast<_float>(iStartY) / static_cast<_float>(iTextureSizeY);
    _float fEndU = static_cast<_float>(iEndX) / static_cast<_float>(iTextureSizeX);
    _float fEndV = static_cast<_float>(iEndY) / static_cast<_float>(iTextureSizeY);

    _float2 vSize = _float2(fSizeX, fSizeY);
    _float4 vUV = _float4(fStartU, fStartV, fEndU, fEndV);

    ATLAS_DATA Data = {};
    Data.vSize = vSize;
    Data.vUV = vUV;

    m_PlayerFontDatas.emplace(chFont, Data);
}

HRESULT CDamageFont::Bind_ShaderResources()
{
    __super::Begin();

    if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    _float fAlpha = m_fLifeTime - m_fCurrentLifeTime;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &fAlpha, sizeof(_float))))
        return E_FAIL;


    return S_OK;
}

void CDamageFont::Update_Buffer(COLLIDER_OWNER eOwner)
{
    m_Vertices.clear();

    string strDamage = to_string(m_iDamage);

    _float fLength = static_cast<_float>(strDamage.length());

    m_fSizeX = 15.f * fLength;
    m_fSizeY = 25.f;

    _uint iIndex = {};

    for (_char chFont : strDamage)
    {
        ATLAS_DATA Data = {};
        
        Data = eOwner == COLLIDER_OWNER::PLAYER ? m_PlayerFontDatas[chFont] : m_MonsterFontDatas[chFont];

        VTXPOSTEX Vertex = {};

        _float fLeftX = static_cast<_float>(iIndex) / (fLength) - 0.5f;
        _float fRightX = static_cast<_float>(iIndex + 1) / (fLength) -0.5f;


        Vertex.vPosition = _float3(fLeftX, 0.5f, 0.f);
        Vertex.vTexcoord = _float2(Data.vUV.x, Data.vUV.y);
        m_Vertices.push_back(Vertex);

        Vertex.vPosition = _float3(fRightX, 0.5f, 0.f);
        Vertex.vTexcoord = _float2(Data.vUV.z, Data.vUV.y);
        m_Vertices.push_back(Vertex);

        Vertex.vPosition = _float3(fRightX, -0.5f, 0.f);
        Vertex.vTexcoord = _float2(Data.vUV.z, Data.vUV.w);
        m_Vertices.push_back(Vertex);

        Vertex.vPosition = _float3(fLeftX, -0.5f, 0.f);
        Vertex.vTexcoord = _float2(Data.vUV.x, Data.vUV.w);
        m_Vertices.push_back(Vertex);

        iIndex++;
    }

    m_pVIBufferCom->Update_Buffer(m_Vertices);
}

CDamageFont* CDamageFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _char* pFontDataFilePath)
{
    CDamageFont* pInstance = new CDamageFont(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pFontDataFilePath)))
    {
        MSG_BOX(TEXT("Failed Created : CDamageFont"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDamageFont::Clone(void* pArg)
{
    CDamageFont* pInstance = new CDamageFont(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CDamageFont"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CDamageFont::Free()
{
    __super::Free();

    Safe_Release(m_pPoolInstance);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
}
