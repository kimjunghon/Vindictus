#include "EnginePch.h"
#include "TextureUI.h"
#include "Texture.h"

CTextureUI::CTextureUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CTextureUI::CTextureUI(const CTextureUI& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CTextureUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTextureUI::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	
	TEXTURE_UI_DESC* pDesc = static_cast<TEXTURE_UI_DESC*>(pArg);

	if (FAILED(Ready_TextueCom(pDesc->iTexturePrototypeLevelIndex, pDesc->strTexturePrototypeTag)))
		return E_FAIL;

	m_IsBlend = pDesc->IsBlend;
	m_fAlpha = pDesc->fAlpha;

	return S_OK;
}

void CTextureUI::Priority_Update(_float fTimeDelta)
{
}

void CTextureUI::Update(_float fTimeDelta)
{
}

void CTextureUI::Late_Update(_float fTimeDelta)
{
}

HRESULT CTextureUI::Render()
{
	return S_OK;
}

HRESULT CTextureUI::Ready_TextueCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	if (FAILED(CGameObject::Add_Component(iTexturePrototypeLevelIndex, strTexturePrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CTextureUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
}
