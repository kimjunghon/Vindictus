#include "ClientPch.h"
#include "Panel.h"

CPanel::CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTextureUI{ pDevice, pDeviceContext }
{
}

CPanel::CPanel(const CPanel& Prototype)
	: CTextureUI{ Prototype }
{
}

HRESULT CPanel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPanel::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CPanel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CPanel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CPanel::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;

	__super::Late_Update(fTimeDelta);
}

HRESULT CPanel::Render()
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

	if (m_IsBlend)
	{
		m_pGameInstance->BSSetState(ENUM_CLASS(D3DBS::ALPHABLEND));
		m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float));	
		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::ALPHABLEND));
	}
	else
		m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::DEFAULT));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	if (m_IsBlend)
		m_pGameInstance->BSSetState(ENUM_CLASS(D3DBS::DEFAULT));

	return S_OK;
}

HRESULT CPanel::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CPanel* CPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel* pInstance = new CPanel(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPanel::Clone(void* pArg)
{
	CPanel* pInstance = new CPanel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CPanel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPanel::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
