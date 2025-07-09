#include "LoadingPoint.h"
#include "GameInstance.h"

CLoadingPoint::CLoadingPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject{ pDevice, pDeviceContext }
{
}

CLoadingPoint::CLoadingPoint(const CLoadingPoint& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CLoadingPoint::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingPoint::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Component()))
		return E_FAIL;

	return S_OK;
}

void CLoadingPoint::Priority_Update(_float fTimeDelta)
{
}

void CLoadingPoint::Update(_float fTimeDelta)
{
}

void CLoadingPoint::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CLoadingPoint::Render()
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

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::LOADINGPOINT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CLoadingPoint::Ready_Component()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_LoadingBar_Point"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CLoadingPoint* CLoadingPoint::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLoadingPoint* pInstance = new CLoadingPoint(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CLoadingPoint"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLoadingPoint::Clone(void* pArg)
{
	CLoadingPoint* pInstance = new CLoadingPoint(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CLoadingPoint"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoadingPoint::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
