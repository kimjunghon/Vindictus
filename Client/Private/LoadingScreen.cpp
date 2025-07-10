#include "LoadingScreen.h"
#include "GameInstance.h"
#include "ProgressBar.h"

CLoadingScreen::CLoadingScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CLoadingScreen::CLoadingScreen(const CLoadingScreen& Prototype)
	: CUIObject { Prototype }
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

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CLoadingScreen::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CLoadingScreen::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLoadingScreen::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;

	__super::Late_Update(fTimeDelta);
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

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iLoadingLevelID)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoadingScreen::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingScreen"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingScreen::Ready_Children()
{
	UIOBJECT_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX - 100.f;
	Children_Desc.fSizeY = 10.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 260.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Back"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(CUIObject::Add_StaticTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_LoadingBar"), &Children_Desc)))
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

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
