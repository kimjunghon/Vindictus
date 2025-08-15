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

	m_pGameInstance->Subscribe<EVENT_NONE_PICK>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_NONE_PICK& Event) {
		this->Event_NonePick(Event); });

	m_pGameInstance->Subscribe<EVENT_PICK_STORAGE>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_PICK_STORAGE& Event) {
		this->Event_PickStorage(Event); });

	m_pGameInstance->Subscribe<EVENT_EQUIP_STORAGE>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_EQUIP_STORAGE& Event) {
		this->Event_EquipStorage(Event); });

	m_pGameInstance->Subscribe<EVENT_PICK_EQUIPMENT>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_PICK_EQUIPMENT& Event) {
		this->Event_PickEquipment(Event); });

	m_pGameInstance->Subscribe<EVENT_UNEQUIP_EQUIPMENT>(ENUM_CLASS(EVENTTYPE::STATIC), [this](const EVENT_UNEQUIP_EQUIPMENT& Event) {
		this->Event_UnEquipEquipment(Event); });


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
		m_IsPickEquipment = false;
	}
}


_bool CMouse::Equip_Item(Shared_ITEM pItem, ITEM_TYPE eItemType, WEAPON_TYPE eWeaponType, ARMOR_TYPE eArmorType, _int iInventoryIndex)
{
	_bool IsEquip = false;

	switch (eItemType)
	{
	case ITEM_TYPE::WEAPON:
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(pItem->second);
		if (pWeapon->Get_WeaponType() == eWeaponType)
		{
			m_pPlayerInstance->EquipWeapon(ENUM_CLASS(pWeapon->Get_WeaponType()), pWeapon, iInventoryIndex);
			IsEquip = true;
		}
		break;
	}
	case ITEM_TYPE::ARMOR:
	{
		CArmor* pArmor = static_cast<CArmor*>(pItem->second);
		if (pArmor->Get_ArmorType() == eArmorType)
		{
			m_pPlayerInstance->EquipArmor(ENUM_CLASS(pArmor->Get_ArmorType()), pArmor, iInventoryIndex);
			IsEquip = true;
		}
		break;
	}
	}

	return IsEquip;
}

void CMouse::UnEquip_Item(ITEM_TYPE eItemType, WEAPON_TYPE eWeaponType, ARMOR_TYPE eArmorType, _int iInventoryIndex)
{
	switch (eItemType)
	{
	case ITEM_TYPE::WEAPON:
		m_pPlayerInstance->UnEquipWeapon(ENUM_CLASS(eWeaponType), iInventoryIndex);
		break;
	case ITEM_TYPE::ARMOR:
		m_pPlayerInstance->UnEquipArmor(ENUM_CLASS(eArmorType), iInventoryIndex);
		break;
	}
}

void CMouse::Publish_PickEquip(_bool IsPick)
{
	EVENT_PICK_EQUIP_ITEM PickEvent = {};
	PickEvent.IsPick = IsPick;
	PickEvent.eItemType = m_EquipItemInfo.eItemType;
	PickEvent.eWeaponType = m_EquipItemInfo.eWeaponType;
	PickEvent.eArmorType = m_EquipItemInfo.eArmorType;

	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), PickEvent);
}

void CMouse::Publish_PickStorage(_bool IsPick)
{
	EVENT_PICK_ITEM PickEvent = {};

	PickEvent.IsPick = IsPick;
	PickEvent.iInventoryIndex = m_iItemIndex;
	m_pGameInstance->Publish(ENUM_CLASS(EVENTTYPE::STATIC), PickEvent);
}

void CMouse::Event_NonePick(const EVENT_NONE_PICK& Event)
{
	if(m_pItem)
	{
		if (m_IsPickEquipment)
			Publish_PickEquip(false);
		else
			Publish_PickStorage(false);
	}

	Clear_ItemData();
}

void CMouse::Event_PickEquipment(const EVENT_PICK_EQUIPMENT& Event)
{
	// 마우스에 Item 없고 장비창 Item Pick
	if (nullptr == m_pItem)
	{
		m_IsPickEquipment = true;

		CGameObject* pItem = { nullptr };

		m_EquipItemInfo.eItemType = Event.eItemType;

		switch (Event.eItemType)
		{
		case ITEM_TYPE::WEAPON:
		{
			pItem = m_pPlayerInstance->UpdatePlayerEquipWeapon(ENUM_CLASS(Event.eWeaponType));
			m_EquipItemInfo.eWeaponType = Event.eWeaponType;
			break;
		}
		case ITEM_TYPE::ARMOR:
		{
			pItem = m_pPlayerInstance->UpdatePlayerEquipArmor(ENUM_CLASS(Event.eArmorType));
			m_EquipItemInfo.eArmorType = Event.eArmorType;
			break;
		}
		}
		if (nullptr == pItem)
			return;

		m_pItem = make_shared<ITEM>(Event.eItemType, pItem);
		Safe_AddRef(pItem);

		Publish_PickEquip(true);
	}
	// 마우스에 Item 있고 장비창 Item Pick
	else			
	{
		// 마우스 Item이 Equipment Item 
		if (m_IsPickEquipment)
		{
			Publish_PickEquip(false);
		}
		// 마우스 Item이 Storage 아이템
		else
		{
			if (m_pItem->first == Event.eItemType)
				Equip_Item(m_pItem, Event.eItemType, Event.eWeaponType, Event.eArmorType, m_iItemIndex);

			Publish_PickStorage(false);
		}

		Clear_ItemData();
	}
}

void CMouse::Event_PickStorage(const EVENT_PICK_STORAGE& Event)
{ 
	if (m_pItem)
	{
		// 마우스에 Item 있고 그 Item이 장비창 Item
		if (m_IsPickEquipment)
		{
			Shared_ITEM pItem = m_pPlayerInstance->GetInventory(Event.iInventoryIndex);
			
			if (nullptr == pItem)
				UnEquip_Item(m_EquipItemInfo.eItemType, m_EquipItemInfo.eWeaponType, m_EquipItemInfo.eArmorType, Event.iInventoryIndex);
			else
			{
				if (pItem->first == m_EquipItemInfo.eItemType)
				{
					_bool IsEquip = false;
					
					IsEquip = Equip_Item(m_pItem, m_EquipItemInfo.eItemType, m_EquipItemInfo.eWeaponType, m_EquipItemInfo.eArmorType, Event.iInventoryIndex);

					if (false == IsEquip)
						UnEquip_Item(m_EquipItemInfo.eItemType, m_EquipItemInfo.eWeaponType, m_EquipItemInfo.eArmorType);

				}
				else
					UnEquip_Item(m_EquipItemInfo.eItemType, m_EquipItemInfo.eWeaponType, m_EquipItemInfo.eArmorType);
			}

			Publish_PickEquip(false);
		}
		// 마우스에 Item 있고 그 Item이 Storage Item
		else if (m_iItemIndex != -1)
		{
			m_pPlayerInstance->Swap_Item(m_iItemIndex, Event.iInventoryIndex);

			Publish_PickStorage(false);
		}

		Clear_ItemData();
	}
	else
	{
		m_pItem = m_pPlayerInstance->GetInventory(Event.iInventoryIndex);
		if (m_pItem)
		{
			m_iItemIndex = Event.iInventoryIndex;
			Safe_AddRef(m_pItem->second);

			Publish_PickStorage(true);
		}
	}
}

void CMouse::Event_EquipStorage(const EVENT_EQUIP_STORAGE& Event)
{
	Shared_ITEM pItem = m_pPlayerInstance->GetInventory(Event.iInventoryIndex);
	
	if (nullptr == pItem)
		return;

	if (pItem->first == ITEM_TYPE::WEAPON)
	{
		CWeapon* pWeapon = static_cast<CWeapon*>(pItem->second);
		Equip_Item(pItem, pItem->first, pWeapon->Get_WeaponType(), ARMOR_TYPE::END, Event.iInventoryIndex);
	}
	else if (pItem->first == ITEM_TYPE::ARMOR)
	{
		CArmor* pArmor = static_cast<CArmor*>(pItem->second);
		Equip_Item(pItem, pItem->first, WEAPON_TYPE::END, pArmor->Get_ArmorType(), Event.iInventoryIndex);
	}

}

void CMouse::Event_UnEquipEquipment(const EVENT_UNEQUIP_EQUIPMENT& Event)
{
	UnEquip_Item(Event.eItemType, Event.eWeaponType, Event.eArmorType);
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

	Clear_ItemData();

	Safe_Release(m_pPlayerInstance);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
