#include "ClientPch.h"
#include "LoadingScreen.h"

CLoadingScreen::CLoadingScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{ pDevice, pDeviceContext }
{
}

CLoadingScreen::CLoadingScreen(const CLoadingScreen& Prototype)
	: CUI_Panel{ Prototype }
{
}

HRESULT CLoadingScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingScreen::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	LOADING_DESC* pDesc = static_cast<LOADING_DESC*>(pArg);

	m_iLoadingLevelID = pDesc->iLoadingLevelID - 2;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CLoadingScreen::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CLoadingScreen::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CLoadingScreen::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CLoadingScreen::Render()
{
	return S_OK;
}

HRESULT CLoadingScreen::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FIRST);
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);

	_tchar szFullPath[MAX_PATH] = TEXT("Prototype_Component_Texture_LoadingScreen%d");

	wsprintf(szFullPath, szFullPath, m_iLoadingLevelID);

	Children_Desc.strTexturePrototypeTag = szFullPath;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeX = m_fSizeX - 100.f;
	Children_Desc.fSizeY = 10.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 260.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingBar"), static_cast<UIOBJECT_DESC*>(&Children_Desc))))
		return E_FAIL;

	return S_OK;
}

CLoadingScreen* CLoadingScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLoadingScreen* pInstance = new CLoadingScreen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CLoadingScreen"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLoadingScreen::Clone(void* pArg)
{
	CLoadingScreen* pInstance = new CLoadingScreen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CLoadingScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingScreen::Free()
{
	__super::Free();

}
