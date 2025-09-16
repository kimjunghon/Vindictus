#include "ClientPch.h"
#include "Slot.h"
#include "Armor.h"
#include "Weapon.h"

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

	_matrix ScalingMatrix = XMMatrixScaling(2.5f, 2.5f, 0.1f);	
	_matrix TransformMatrix = XMMatrixTranslation(m_fX - (m_iWinSizeX * 0.5f) + 17.f, -m_fY + (m_iWinSizeY * 0.5f) - 10.f, (UI_FAR / (_float)m_iDepth));
	_matrix WorldMatrix = ScalingMatrix * TransformMatrix;

	XMStoreFloat4x4(&m_SlotRenderDesc.WorldMatrix, WorldMatrix);
	m_SlotRenderWorldMatrix = m_SlotRenderDesc.WorldMatrix;

	m_SlotRenderDesc.ViewMatrix = m_ViewMatrix;
	
	D3D11_VIEWPORT			Viewport{};
	_uint			iNumViewports = { 1 };
	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);
	XMStoreFloat4x4(&m_SlotRenderDesc.ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 100.f));

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
		if (m_IsPick)
		{
			POINT ptMouse = m_pGameInstance->Get_MousePoint();
			_float fX = static_cast<_float>(ptMouse.x);
			_float fY = static_cast<_float>(ptMouse.y);

			_matrix ScalingMatrix = XMMatrixScaling(2.f, 2.f, 0.1f);
			_matrix TransformMatrix = XMMatrixTranslation(fX - (m_iWinSizeX * 0.5f) + 10.f, -fY + (m_iWinSizeY * 0.5f) + 5.f, (UI_FAR / (_float)m_iDepth));
			_matrix WorldMatrix = ScalingMatrix * TransformMatrix;

			XMStoreFloat4x4(&m_MouseRenderWorldMatirx, WorldMatrix);

			m_SlotRenderDesc.WorldMatrix = m_MouseRenderWorldMatirx;
		}
		else
			m_SlotRenderDesc.WorldMatrix = m_SlotRenderWorldMatrix;

		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
			return;
	}
}

HRESULT CSlot::Render()
{
	switch(m_pItem->first)
	{
	case ITEM_TYPE::WEAPON:
		m_RenderSlot.pRenderWeapon->RenderSlot(m_SlotRenderDesc);
		break;
	case ITEM_TYPE::ARMOR:
		m_RenderSlot.pRenderArmor->RenderSlot(m_SlotRenderDesc);
		break;
	}

	return S_OK;
}

HRESULT CSlot::UpdateItem(Shared_ITEM pItem)
{
	ClearItem();
	
	m_pItem = pItem;
	
	if(m_pItem)
	{
		Safe_AddRef(m_pItem->second);

		switch (m_pItem->first)
		{
		case ITEM_TYPE::WEAPON:
			m_RenderSlot.pRenderWeapon = static_cast<CWeapon*>(m_pItem->second);
			break;
		case ITEM_TYPE::ARMOR:
			m_RenderSlot.pRenderArmor = static_cast<CArmor*>(m_pItem->second);
			break;
		//case ITEM_TYPE::OTHERS:
		}
	}

	return S_OK;
}

void CSlot::ClearItem()
{
	if (nullptr == m_pItem)
		return;

	ClearRenderSlot();
	Safe_Release(m_pItem->second);
	m_pItem = nullptr;
}

void CSlot::ClearRenderSlot()
{
	m_RenderSlot.pRenderWeapon = nullptr;
	m_RenderSlot.pRenderArmor = nullptr;
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
}
