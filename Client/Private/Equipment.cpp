#include "ClientPch.h"
#include "Equipment.h"
#include "Slot.h"
#include "Armor.h"
#include "Weapon.h"

CEquipment::CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CUI_Panel { pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
}

CEquipment::CEquipment(const CEquipment& Prototype)
	: CUI_Panel{ Prototype }
	, m_pPlayerInstance { Prototype.m_pPlayerInstance }
	, m_iNumSlotsX { Prototype.m_iNumSlotsX }
	, m_iNumSlotsY { Prototype.m_iNumSlotsY }
	, m_fSlotStartOffsetX { Prototype.m_fSlotStartOffsetX }
	, m_fSlotStartOffsetY { Prototype.m_fSlotStartOffsetY }
	, m_fSlotOffsetX { Prototype.m_fSlotOffsetX }
	, m_fSlotOffsetY { Prototype.m_fSlotOffsetY }
	, m_iIndexFlag { Prototype.m_iIndexFlag}
	, m_PickEvents { Prototype.m_PickEvents}
	, m_UnEqupEvents { Prototype.m_UnEqupEvents }
	, m_WeaponSlotIndex { Prototype.m_WeaponSlotIndex }
	, m_ArmorSlotIndex { Prototype.m_ArmorSlotIndex }
{
	Safe_AddRef(m_pPlayerInstance);
}

HRESULT CEquipment::Initialize_Prototype()
{
	m_iIndexFlag = (1 << ENUM_CLASS(HEAD)) | (1 << ENUM_CLASS(UPPER)) | (1 << ENUM_CLASS(LOWER)) | (1 << ENUM_CLASS(HAND)) | (1 << ENUM_CLASS(FOOT)) |
		(1 << ENUM_CLASS(SWORD)) | (1 << ENUM_CLASS(SHILED));
	
	
	m_iNumSlotsX = ENUM_CLASS(EQUIPMENT::END) / 4;
	m_iNumSlotsY = ENUM_CLASS(EQUIPMENT::END) / 3;

	m_fSlotStartOffsetX = -71.5f;
	m_fSlotStartOffsetY = -148.f;

	m_fSlotOffsetX = 56.f;
	m_fSlotOffsetY = 55.f;

	if (FAILED(Ready_Events()))
		return E_FAIL;

	if (FAILED(Ready_SlotIndex()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipment::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_PICK_EQUIP_ITEM>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_PICK_EQUIP_ITEM& Event) {
		this->Event_Pick_Equip_Item(Event); });

	m_pGameInstance->Subscribe<EVENT_CHANGE_WEAPON>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_CHANGE_WEAPON& Event) {
		this->Event_Change_Weapon(Event); });

	m_pGameInstance->Subscribe<EVENT_CHANGE_ARMOR>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_CHANGE_ARMOR& Event) {
		this->Event_Change_Armor(Event); });

    return S_OK;
}

void CEquipment::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CEquipment::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CEquipment::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CEquipment::Render()
{
    return S_OK;
}

HRESULT CEquipment::Ready_Children()
{
	CTextureUI::TEXTURE_UI_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = m_fSizeX;
	Children_Desc.fSizeY = m_fSizeY;
	Children_Desc.fOffsetX = 0.f;
	Children_Desc.fOffsetY = 0.f;
	Children_Desc.iTexturePrototypeLevelIndex = ENUM_CLASS(LEVEL::STATIC);
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Inventroy_Background");
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::SECOND);
	Children_Desc.IsBlend = true;
	Children_Desc.fAlpha = 0.8f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	Children_Desc.fSizeX -= 50.f;
	Children_Desc.fSizeY -= 40.f;
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Equipment");
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	if (FAILED(Ready_Slots()))
		return E_FAIL;
    return S_OK;
}

HRESULT CEquipment::Ready_Slots()
{
	CUIObject::UIOBJECT_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 48.5f;
	Children_Desc.fSizeY = 48.5f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);

	for (_uint i = 0; i < m_iNumSlotsY; i++)
	{
		for (_uint j = 0; j < m_iNumSlotsX; j++)
		{
			_uint iIndex = {};
			iIndex = (i * m_iNumSlotsX) + j;

			Children_Desc.fOffsetX = m_fSlotStartOffsetX + (m_fSlotOffsetX * static_cast<_float>(j));
			Children_Desc.fOffsetY = m_fSlotStartOffsetY + (m_fSlotOffsetY * static_cast<_float>(i));

			CSlot* pSlot = nullptr;

			if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Slot"), &Children_Desc, reinterpret_cast<CUIObject**>(&pSlot))))
				return E_FAIL;

			m_Slots[iIndex] = pSlot;
		}
	}

	_float fLeft = m_fX + m_fSlotStartOffsetX;
	_float fTop = m_fY + m_fSlotStartOffsetY;
	_float fRight = fLeft + (m_fSlotOffsetX * static_cast<_float>(m_iNumSlotsX));
	_float fBottom = fTop + (m_fSlotOffsetY * static_cast<_float>(m_iNumSlotsY));

	m_SlotRect.left = static_cast<LONG>(fLeft);
	m_SlotRect.top = static_cast<LONG>(fTop);
	m_SlotRect.right = static_cast<LONG>(fRight);
	m_SlotRect.bottom = static_cast<LONG>(fBottom);

    return S_OK;
}

HRESULT CEquipment::Ready_Events()
{
#pragma region PICK_EVENT
#pragma region ARMOR MAPPING
	// HEAD
	EVENT_PICK_EQUIPMENT PickEvent = {};
	PickEvent.eItemType = ITEM_TYPE::ARMOR;
	PickEvent.eArmorType = ARMOR_TYPE::HEAD;
	PickEvent.eWeaponType = WEAPON_TYPE::END;
	m_PickEvents[ENUM_CLASS(HEAD)] = PickEvent;

	// UPPER
	PickEvent.eArmorType = ARMOR_TYPE::UPPER;
	m_PickEvents[ENUM_CLASS(UPPER)] = PickEvent;

	// LOWER
	PickEvent.eArmorType = ARMOR_TYPE::LOWER;
	m_PickEvents[ENUM_CLASS(LOWER)] = PickEvent;

	// HAND
	PickEvent.eArmorType = ARMOR_TYPE::HAND;
	m_PickEvents[ENUM_CLASS(HAND)] = PickEvent;

	// FOOT
	PickEvent.eArmorType = ARMOR_TYPE::FOOT;
	m_PickEvents[ENUM_CLASS(FOOT)] = PickEvent;
#pragma endregion

#pragma region WEAPON MAPPING
	// SWORD
	PickEvent.eItemType = ITEM_TYPE::WEAPON;
	PickEvent.eArmorType = ARMOR_TYPE::END;
	PickEvent.eWeaponType = WEAPON_TYPE::SWORD;

	m_PickEvents[ENUM_CLASS(SWORD)] = PickEvent;

	// SHILED
	PickEvent.eWeaponType = WEAPON_TYPE::SHILED;
	m_PickEvents[ENUM_CLASS(SHILED)] = PickEvent;

#pragma endregion
#pragma endregion

#pragma region UNEQUIP_EVENT
#pragma region ARMOR MAPPING
	EVENT_UNEQUIP_EQUIPMENT UnEquipEvent = {};
	UnEquipEvent.eItemType = ITEM_TYPE::ARMOR;
	UnEquipEvent.eArmorType = ARMOR_TYPE::HEAD;
	UnEquipEvent.eWeaponType = WEAPON_TYPE::END;
	m_UnEqupEvents[ENUM_CLASS(HEAD)] = UnEquipEvent;

	// UPPER
	UnEquipEvent.eArmorType = ARMOR_TYPE::UPPER;
	m_UnEqupEvents[ENUM_CLASS(UPPER)] = UnEquipEvent;

	// LOWER
	UnEquipEvent.eArmorType = ARMOR_TYPE::LOWER;
	m_UnEqupEvents[ENUM_CLASS(LOWER)] = UnEquipEvent;

	// HAND
	UnEquipEvent.eArmorType = ARMOR_TYPE::HAND;
	m_UnEqupEvents[ENUM_CLASS(HAND)] = UnEquipEvent;

	// FOOT
	UnEquipEvent.eArmorType = ARMOR_TYPE::FOOT;
	m_UnEqupEvents[ENUM_CLASS(FOOT)] = UnEquipEvent;

#pragma endregion
#pragma region WEAPON MAPPING
	UnEquipEvent.eItemType = ITEM_TYPE::WEAPON;
	UnEquipEvent.eArmorType = ARMOR_TYPE::END;
	UnEquipEvent.eWeaponType = WEAPON_TYPE::SWORD;
	m_UnEqupEvents[ENUM_CLASS(SWORD)] = UnEquipEvent;

	// UPPER
	UnEquipEvent.eWeaponType = WEAPON_TYPE::SHILED;
	m_UnEqupEvents[ENUM_CLASS(SHILED)] = UnEquipEvent;

#pragma endregion

#pragma endregion
	return S_OK;
}

HRESULT CEquipment::Ready_SlotIndex()
{
	m_WeaponSlotIndex[WEAPON_TYPE::SWORD] = ENUM_CLASS(SWORD);
	m_WeaponSlotIndex[WEAPON_TYPE::SHILED] = ENUM_CLASS(SHILED);

	m_ArmorSlotIndex[ARMOR_TYPE::HEAD] = ENUM_CLASS(HEAD);
	m_ArmorSlotIndex[ARMOR_TYPE::UPPER] = ENUM_CLASS(UPPER);
	m_ArmorSlotIndex[ARMOR_TYPE::LOWER] = ENUM_CLASS(LOWER);
	m_ArmorSlotIndex[ARMOR_TYPE::HAND] = ENUM_CLASS(HAND);
	m_ArmorSlotIndex[ARMOR_TYPE::FOOT] = ENUM_CLASS(FOOT);

	return S_OK;
}


void CEquipment::Event_Pick_Equip_Item(const EVENT_PICK_EQUIP_ITEM& Event)
{
	_uint iIndex = {};

	switch (Event.eItemType)
	{
	case ITEM_TYPE::WEAPON:
		iIndex = m_WeaponSlotIndex[Event.eWeaponType];
		break;
	case ITEM_TYPE::ARMOR:
		iIndex = m_ArmorSlotIndex[Event.eArmorType];
		break;
	}

	m_Slots[iIndex]->IsPick(Event.IsPick);
}

void CEquipment::Event_Change_Weapon(const EVENT_CHANGE_WEAPON& Event)
{
	_uint iIndex = m_WeaponSlotIndex[static_cast<WEAPON_TYPE>(Event.iWeaponTypeIndex)];

	Shared_ITEM pItem = nullptr;

	CWeapon* pWeapon = m_pPlayerInstance->UpdatePlayerEquipWeapon(Event.iWeaponTypeIndex);

	if (nullptr != pWeapon)
		pItem = make_shared<ITEM>(ITEM_TYPE::WEAPON, pWeapon);

	m_Slots[iIndex]->UpdateItem(pItem);
}

void CEquipment::Event_Change_Armor(const EVENT_CHANGE_ARMOR& Event)
{
	_uint iIndex = m_ArmorSlotIndex[static_cast<ARMOR_TYPE>(Event.iArmorTypeIndex)];

	Shared_ITEM pItem = nullptr;

	CArmor* pArmor = m_pPlayerInstance->UpdatePlayerEquipArmor(Event.iArmorTypeIndex);
	
	if (nullptr != pArmor)
		pItem = make_shared<ITEM>(ITEM_TYPE::ARMOR, pArmor);
	
	m_Slots[iIndex]->UpdateItem(pItem);
}

_bool CEquipment::IsPick(HWND hWnd)
{
	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	if (PtInRect(&m_SlotRect, ptMouse))
	{
		_float fX = static_cast<_float>(ptMouse.x - m_SlotRect.left);
		_float fY = static_cast<_float>(ptMouse.y - m_SlotRect.top);

		m_iPickIndex = static_cast<_uint>((fX / m_fSlotOffsetX)) + ((m_iNumSlotsX) * static_cast<_uint>(fY / m_fSlotOffsetY));

		// Index 유효 체크
		if (m_iIndexFlag & (1 << m_iPickIndex))
		{
			EVENT_PICK_EQUIPMENT Event = {};
			Event = m_PickEvents[m_iPickIndex];

			m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

			return true;
		}
	}
	return false;
}

_bool CEquipment::IsUnEquip(HWND hWnd)
{
	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	if (PtInRect(&m_SlotRect, ptMouse))
	{
		_float fX = static_cast<_float>(ptMouse.x - m_SlotRect.left);
		_float fY = static_cast<_float>(ptMouse.y - m_SlotRect.top);

		m_iPickIndex = static_cast<_uint>((fX / m_fSlotOffsetX)) + ((m_iNumSlotsX) * static_cast<_uint>(fY / m_fSlotOffsetY));

		// Index 유효 체크
		if (m_iIndexFlag & (1 << m_iPickIndex))
		{
			EVENT_UNEQUIP_EQUIPMENT Event = {};
			Event = m_UnEqupEvents[m_iPickIndex];

			m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

			return true;
		}
	}
	return false;
}

CEquipment* CEquipment::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CEquipment* pInstance = new CEquipment(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CEquipment"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEquipment::Clone(void* pArg)
{
	CEquipment* pInstance = new CEquipment(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CEquipment"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEquipment::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerInstance);

	for (auto& pSlot : m_Slots)
		Safe_Release(pSlot);
}