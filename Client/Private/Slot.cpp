#include "ClientPch.h"
#include "Slot.h"

CSlot::CSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
{
}

CSlot::CSlot(const CSlot& Prototype)
	: CUIObject { Prototype }
{
}

HRESULT CSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlot::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	//TEST
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CSlot::Priority_Update(_float fTimeDelta)
{
}

void CSlot::Update(_float fTimeDelta)
{
}

void CSlot::Late_Update(_float fTimeDelta)
{
	if(m_pItem)
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::NONBLEND, this)))
			return;
	}
}

HRESULT CSlot::Render()
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

HRESULT CSlot::UpdateItem(Shared_ITEM pItem)
{
	ClearItem();
	
	m_pItem = pItem;
	
	if(m_pItem)
		Safe_AddRef(m_pItem->second);

	return S_OK;
}

void CSlot::ClearItem()
{
	if (m_pItem)
	{
		Safe_Release(m_pItem->second);
		m_pItem = nullptr;
	}
}

HRESULT CSlot::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_OptionButton"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

CSlot* CSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSlot* pInstance = new CSlot(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CSlot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CSlot::Clone(void* pArg)
{
	CSlot* pInstance = new CSlot(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CSlot"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSlot::Free()
{
	__super::Free();

	ClearItem();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
