#include "ClientPch.h"
#include "Bar.h"

CBar::CBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CTextureUI { pDevice, pDeviceContext}
{
}

CBar::CBar(const CBar& Prototype)
	: CTextureUI { Prototype }
{
}

HRESULT CBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBar::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fStartX = m_fX - (m_fSizeX * 0.5f);

    return S_OK;
}

void CBar::Priority_Update(_float fTimeDelta)
{
}

void CBar::Update(_float fTimeDelta)
{
}

void CBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CBar::Render()
{
	__super::Begin();

	if (FAILED(m_pTransformCom->Bind_Shader_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fStartX", &m_fStartX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fProgressBarRatio", &m_fRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	m_pShaderCom->Begin(ENUM_CLASS(SHADER_VTXPOSTEX::PROGRESSBAR));

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

void CBar::Set_Ratio(_float fRatio)
{
	m_fRatio = fRatio;
}

HRESULT CBar::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

    return S_OK;
}

CBar* CBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBar* pInstance = new CBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBar"));
		Safe_Release(pInstance);
	}
    return pInstance;
}

CGameObject* CBar::Clone(void* pArg)
{
	CBar* pInstance = new CBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBar::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
