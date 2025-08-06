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

	if (FAILED(Ready_Children()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
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

	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;

	_uint iTextureFlag = (*m_iUIState);
	iTextureFlag ^= ENUM_CLASS(STATE_FLAG::LOADING);
	m_iTextureIndex = ComputeBitIndex(iTextureFlag);
}

HRESULT CLoadingScreen::Render()
{
	__super::Begin();

	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoadingScreen::Ready_Children()
{
	CUIObject::UIOBJECT_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX - 100.f;
	Children_Desc.fSizeY = 10.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 260.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingBar"), &Children_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingScreen::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingScreen"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}

_uint CLoadingScreen::ComputeBitIndex(_uint iFlag)
{
	_uint iIndex = 0;

	while (iFlag >>= 1)
		iIndex++;

	return iIndex - 4;
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

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
