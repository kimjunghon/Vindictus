#include "ClientPch.h"
#include "Mouse.h"
#include "Weapon.h"
#include "Armor.h"

CMouse::CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUIObject { pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
}

CMouse::CMouse(const CMouse& Prototype)
	: CUIObject { Prototype }
	, m_pPlayerInstance{ Prototype.m_pPlayerInstance }
{
	Safe_AddRef(m_pPlayerInstance);
}

HRESULT CMouse::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMouse::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	UI_MOUSE_DESC* pDesc = static_cast<UI_MOUSE_DESC*>(pArg);
	m_pUIState = pDesc->StateDesc.iUIState;
	
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_PICK_STORAGE>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_PICK_STORAGE& Event) {
		this->Event_PickInventory(Event); });

	return S_OK;
}

void CMouse::Priority_Update(_float fTimeDelta)
{
	if (*m_pUIState & ~ENUM_CLASS(STATE_FLAG::GAMEPLAY))
	{
		POINT	ptMouse = {};
		GetCursorPos(&ptMouse);
		ScreenToClient(g_hWnd, &ptMouse);

		_float fMouseX = (_float)(ptMouse.x);
		_float fMouseY = (_float)(ptMouse.y);

		Set_Position(fMouseX, fMouseY);

		POINT ptUIMouse = POINT{ static_cast<LONG>(m_fX), static_cast<LONG>(m_fY) };
		m_pGameInstance->Set_MousePoint(ptUIMouse);
	}
}

void CMouse::Update(_float fTimeDelta)
{
	if (*m_pUIState & ~ENUM_CLASS(STATE_FLAG::GAMEPLAY))
	{

	}
}

void CMouse::Late_Update(_float fTimeDelta)
{
	if (*m_pUIState & ~ENUM_CLASS(STATE_FLAG::GAMEPLAY))
	{
		if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
			return;
	}
}

HRESULT CMouse::Render()
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

HRESULT CMouse::Ready_Components()
{
	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_GamePlay_Cursor"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

void CMouse::Clear_ItemData()
{
	if(m_pItem)
	{
		Safe_Release(m_pItem->second);
		m_pItem = nullptr;
		m_iItemIndex = -1;
	}
}

void CMouse::Event_PickEquipment(const EVENT_PICK_EQUIPMENT& Event)
{
	if (nullptr == m_pItem)
		return;

	if (m_pItem->first != Event.eItemType)
	{
		Clear_ItemData();
		return;
	}

	_bool IsEquip = false;
	
	switch (Event.eItemType)
	{
	case ITEM_TYPE::WEAPON:
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(m_pItem->second);
		if (pWeapon->Get_WeaponType() == Event.eWeaponType)
		{
			m_pPlayerInstance->EquipWeapon(ENUM_CLASS(pWeapon->Get_WeaponType()), pWeapon);
			IsEquip = true;
		}
		break;
	}
	case ITEM_TYPE::ARMOR:
	{
		CArmor* pArmor = static_cast<CArmor*>(m_pItem->second);
		if (pArmor->Get_ArmorType() == Event.eArmorType)
		{
			m_pPlayerInstance->EquipArmor(ENUM_CLASS(pArmor->Get_ArmorType()), pArmor);
			IsEquip = true;
		}
		break;
	}
	}

	if(IsEquip)
		Clear_ItemData();
	else
	{

	}
}

void CMouse::Event_PickInventory(const EVENT_PICK_STORAGE& Event)
{
	if (m_iItemIndex != -1)
	{
		m_pPlayerInstance->Swap_Item(m_iItemIndex, Event.iInventoryIndex);
		Clear_ItemData();
	}
	else
	{
		m_pItem = m_pPlayerInstance->GetInventory(Event.iInventoryIndex);
		if (m_pItem)
		{
			m_iItemIndex = Event.iInventoryIndex;
			Safe_AddRef(m_pItem->second);
		}
	}
}

CMouse* CMouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMouse* pInstance = new CMouse(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CMouse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMouse::Clone(void* pArg)
{
	CMouse* pInstance = new CMouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMouse"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMouse::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerInstance);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
