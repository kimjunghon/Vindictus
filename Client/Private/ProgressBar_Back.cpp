#include "ProgressBar_Back.h"
#include "GameInstance.h"

CProgressBar_Back::CProgressBar_Back(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject{ pDevice, pDeviceContext }
{
}

CProgressBar_Back::CProgressBar_Back(const CProgressBar_Back& Prototype)
	: CUIObject{ Prototype }
{
}

HRESULT CProgressBar_Back::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProgressBar_Back::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CProgressBar_Back::Priority_Update(_float fTimeDelta)
{
}

void CProgressBar_Back::Update(_float fTimeDelta)
{
}

void CProgressBar_Back::Late_Update(_float fTimeDelta)
{
	if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
		return;
}

HRESULT CProgressBar_Back::Render()
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

HRESULT CProgressBar_Back::Ready_TextureCom(_uint iTexturePrototypeLevelIndex, const _wstring& strTexturePrototypeTag)
{
	if (FAILED(CGameObject::Add_Component(iTexturePrototypeLevelIndex, strTexturePrototypeTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CProgressBar_Back::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

CProgressBar_Back* CProgressBar_Back::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CProgressBar_Back* pInstance = new CProgressBar_Back(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CProgressBar_Back"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProgressBar_Back::Clone(void* pArg)
{
	CProgressBar_Back* pInstance = new CProgressBar_Back(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CProgressBar_Back"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProgressBar_Back::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
