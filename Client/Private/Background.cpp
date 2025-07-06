#include "Background.h"
#include "GameInstance.h"

CBackground::CBackground(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext}
{
}

CBackground::CBackground(const CBackground& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CBackground::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBackground::Initialize(void* pArg)
{
	UIOBJECT_DESC               Desc{};
	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = 200.0f;
	Desc.fSizeY = 200.0f;
	Desc.fOffsetX = 0.f;
	Desc.fOffsetY = 0.f;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CBackground::Priority_Update(_float fTimeDelta)
{
}

void CBackground::Update(_float fTimeDelta)
{
}

void CBackground::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CBackground::Render()
{
	__super::Begin();

	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBackground::Ready_Components()
{
	if(FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if(FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CBackground* CBackground::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBackground* pInstance = new CBackground(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBackground::Clone(void* pArg)
{
	CBackground* pInstance = new CBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBackground::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);

}
