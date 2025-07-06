#include "LogoScreen.h"
#include "GameInstance.h"

CLogoScreen::CLogoScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject{ pDevice, pDeviceContext }
{
}

CLogoScreen::CLogoScreen(const CLogoScreen& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CLogoScreen::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLogoScreen::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	if (FAILED(Ready_Button()))
		return E_FAIL;

	return S_OK;
}

void CLogoScreen::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CLogoScreen::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CLogoScreen::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;

	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CLogoScreen::Render()
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

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLogoScreen::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_SkyBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoScreen::Ready_Children()
{
	UIOBJECT_DESC Children_Desc{};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 100.f;
	Children_Desc.fSizeY = 100.f;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"),
		ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_WaterMark"), Children_Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogoScreen::Ready_Button()
{
	CButtonObject::BUTTONOBJECT_DESC Button_Desc{};
	Button_Desc.fX = m_fX;
	Button_Desc.fY = m_fY;
	Button_Desc.fSizeX = 100.f;
	Button_Desc.fSizeY = 100.f;
	Button_Desc.fOffsetX = 250.f;
	Button_Desc.fOffsetY = 100.f;
	Button_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Button_Desc.Callback = [&]() {
		EVENT_LEVEL_CHANGE Event;
		Event.iChange_Level = ENUM_CLASS(LEVEL::GAMEPLAY);
		Event.bIsLoading = false;
		m_pGameInstance->Publish(ENUM_CLASS(LEVEL::STATIC), Event);
		};

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_ButtonObject_Button"),
		ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Button"), Button_Desc)))
		return E_FAIL;

	Button_Desc.fOffsetX = 250.f;
	Button_Desc.fOffsetY = 250.f;
	Button_Desc.Callback = []() {
			DestroyWindow(g_hWnd);
		};

	if (FAILED(CUIObject::Add_DynamicTexture_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_ButtonObject_Button"),
		ENUM_CLASS(LEVEL::LOGO), TEXT("Prototype_Component_Texture_Button"), Button_Desc)))
		return E_FAIL;

	return S_OK;
}

CLogoScreen* CLogoScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLogoScreen* pInstance = new CLogoScreen(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogoScreen::Clone(void* pArg)
{
	CLogoScreen* pInstance = new CLogoScreen(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CLogoScreen"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogoScreen::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
