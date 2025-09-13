#include "ClientPch.h"
#include "BossHPBar.h"

CBossHPBar::CBossHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CBar { pDevice, pDeviceContext }
{
}

CBossHPBar::CBossHPBar(const CBossHPBar& Prototype)
	: CBar { Prototype }
	, m_iPassIndex { Prototype.m_iPassIndex}
	, m_iTextureIndex { Prototype.m_iTextureIndex }
{
}

HRESULT CBossHPBar::Initialize_Prototype()
{
	m_iPassIndex = ENUM_CLASS(SHADER_VTXPOSTEX::BLEND_BAR);
	m_iTextureIndex = 2;

	return S_OK;
}

HRESULT CBossHPBar::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CBossHPBar::Priority_Update(_float fTimeDelta)
{
}

void CBossHPBar::Update(_float fTimeDelta)
{
}

void CBossHPBar::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CBossHPBar::Render()
{
	__super::Begin();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iPassIndex);

	m_pVIBufferCom->Bind_Resources();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBossHPBar::Bind_ShaderResources()
{
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

	if (FAILED(m_pTextureCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	return S_OK;
}

CBossHPBar* CBossHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBossHPBar* pInstance = new CBossHPBar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CBossHPBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBossHPBar::Clone(void* pArg)
{
	CBossHPBar* pInstance = new CBossHPBar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CBossHPBar"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBossHPBar::Free()
{
	__super::Free();
}
