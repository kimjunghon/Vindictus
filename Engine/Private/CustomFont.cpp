#include "EnginePch.h"
#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : m_pDevice { pDevice }
    , m_pDeviceContext { pDeviceContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pDeviceContext);
}

HRESULT CCustomFont::Initialize(const _tchar* pFontFilePath)
{
    m_pBatch = new SpriteBatch(m_pDeviceContext);
    m_pFont = new SpriteFont(m_pDevice, pFontFilePath);

    if (nullptr == m_pBatch)
        return E_FAIL;

    if (nullptr == m_pFont)
        return E_FAIL;

    return S_OK;
}

void CCustomFont::DrawFont(const _tchar* pText, const _float2& vPosition, _fvector vColor, _float fRadian, const _float2& vOrigin, const _float2& vScale)
{
    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, pText, vPosition, vColor, fRadian, vOrigin, vScale);

    m_pBatch->End();
}

CCustomFont* CCustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pFontFilePath)
{
    CCustomFont* pInstance = new CCustomFont(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize(pFontFilePath)))
    {
        MSG_BOX(TEXT("Failed Created : CCustomFont"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCustomFont::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pDeviceContext);

    Safe_Delete(m_pBatch);
    Safe_Delete(m_pFont);
}
