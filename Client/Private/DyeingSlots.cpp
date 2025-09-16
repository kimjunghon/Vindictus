#include "ClientPch.h"
#include "DyeingSlots.h"
#include "Slot.h"
#include "Armor.h"
#include "Weapon.h"

CDyeingSlots::CDyeingSlots(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel { pDevice, pDeviceContext }
	, m_pPlayerInstance{ CPlayerInstance::GetInstance() }
{
	Safe_AddRef(m_pPlayerInstance);
}

CDyeingSlots::CDyeingSlots(const CDyeingSlots& Prototype)
	: CUI_Panel { Prototype }
	, m_pPlayerInstance{ Prototype.m_pPlayerInstance }
	, m_iNumSlotsX{ Prototype.m_iNumSlotsX }
	, m_iNumSlotsY{ Prototype.m_iNumSlotsY }
	, m_fSlotStartOffsetX{ Prototype.m_fSlotStartOffsetX }
	, m_fSlotStartOffsetY{ Prototype.m_fSlotStartOffsetY }
	, m_fSlotOffsetX{ Prototype.m_fSlotOffsetX }
	, m_fSlotOffsetY{ Prototype.m_fSlotOffsetY }
	, m_iArmorFlag{ Prototype.m_iArmorFlag }
	, m_iWeaponFlag{ Prototype.m_iWeaponFlag }
	, m_WeaponSlotIndex{ Prototype.m_WeaponSlotIndex }
	, m_ArmorSlotIndex{ Prototype.m_ArmorSlotIndex }
{
	Safe_AddRef(m_pPlayerInstance);
}

HRESULT CDyeingSlots::Initialize_Prototype()
{
	m_iArmorFlag = (1 << ENUM_CLASS(HEAD)) | (1 << ENUM_CLASS(UPPER)) | (1 << ENUM_CLASS(LOWER)) | (1 << ENUM_CLASS(HAND)) | (1 << ENUM_CLASS(FOOT));
	m_iWeaponFlag = (1 << ENUM_CLASS(SWORD)) | (1 << ENUM_CLASS(SHILED));


	m_iNumSlotsX = ENUM_CLASS(EQUIPMENT::END) / 4;
	m_iNumSlotsY = ENUM_CLASS(EQUIPMENT::END) / 3;

	m_fSlotStartOffsetX = -71.5f;
	m_fSlotStartOffsetY = -148.f;

	m_fSlotOffsetX = 56.f;
	m_fSlotOffsetY = 55.f;

	if (FAILED(Ready_SlotIndex()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDyeingSlots::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	return S_OK;
}

void CDyeingSlots::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CDyeingSlots::Update(_float fTimeDelta)
{
	__super::Children_Update(fTimeDelta);
}

void CDyeingSlots::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CDyeingSlots::Render()
{
	return S_OK;
}

HRESULT CDyeingSlots::Ready_Children()
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

HRESULT CDyeingSlots::Ready_Slots()
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

			if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Slot"), &Children_Desc, reinterpret_cast<CUIObject**>(&m_Slots[iIndex]))))
				return E_FAIL;

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

HRESULT CDyeingSlots::Ready_SlotIndex()
{
	m_WeaponSlotIndex[ENUM_CLASS(SWORD)] = WEAPON_TYPE::SWORD;
	m_WeaponSlotIndex[ENUM_CLASS(SHILED)] = WEAPON_TYPE::SHILED;

	m_ArmorSlotIndex[ENUM_CLASS(HEAD)] = ARMOR_TYPE::HEAD;
	m_ArmorSlotIndex[ENUM_CLASS(UPPER)] = ARMOR_TYPE::UPPER;
	m_ArmorSlotIndex[ENUM_CLASS(LOWER)] = ARMOR_TYPE::LOWER;
	m_ArmorSlotIndex[ENUM_CLASS(HAND)] = ARMOR_TYPE::HAND;
	m_ArmorSlotIndex[ENUM_CLASS(FOOT)] = ARMOR_TYPE::FOOT;

	return S_OK;
}

void CDyeingSlots::Update_Slots()
{
	for (auto& Pair : m_WeaponSlotIndex)
	{
		CWeapon* pWeapon = m_pPlayerInstance->UpdatePlayerEquipWeapon(ENUM_CLASS(Pair.second));
		if (nullptr == pWeapon)
			continue;

		Shared_ITEM pItem = nullptr;

		pItem = make_shared<ITEM>(ITEM_TYPE::WEAPON, pWeapon);

		m_Slots[Pair.first]->UpdateItem(pItem);
	}

	for (auto& Pair : m_ArmorSlotIndex)
	{
		CArmor* pArmor = m_pPlayerInstance->UpdatePlayerEquipArmor(ENUM_CLASS(Pair.second));
		if (nullptr == pArmor)
			continue;

		Shared_ITEM pItem = nullptr;

		pItem = make_shared<ITEM>(ITEM_TYPE::ARMOR, pArmor);

		m_Slots[Pair.first]->UpdateItem(pItem);
	}
}

void CDyeingSlots::Clear_Slots()
{
	for (auto& Pair : m_WeaponSlotIndex)
		m_Slots[Pair.first]->ClearItem();

	for (auto& Pair : m_ArmorSlotIndex)
		m_Slots[Pair.first]->ClearItem();
}

pair<ITEM_TYPE, _uint>* CDyeingSlots::IsPickSlot(HWND hWnd)
{
	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	if (PtInRect(&m_SlotRect, ptMouse))
	{
		_float fX = static_cast<_float>(ptMouse.x - m_SlotRect.left);
		_float fY = static_cast<_float>(ptMouse.y - m_SlotRect.top);

		m_iPickIndex = static_cast<_uint>((fX / m_fSlotOffsetX)) + ((m_iNumSlotsX) * static_cast<_uint>(fY / m_fSlotOffsetY));

		// Index 유효 체크
		if (m_iArmorFlag & (1 << m_iPickIndex))
		{
			m_SlotItemType.first = ITEM_TYPE::ARMOR;
			m_SlotItemType.second = ENUM_CLASS(m_ArmorSlotIndex[m_iPickIndex]);
			return &m_SlotItemType;
		}
		else if (m_iWeaponFlag & (1 << m_iPickIndex))
		{
			m_SlotItemType.first = ITEM_TYPE::WEAPON;
			m_SlotItemType.second = ENUM_CLASS(m_WeaponSlotIndex[m_iPickIndex]);
			return &m_SlotItemType;
		}
	}

	return nullptr;
}

CDyeingSlots* CDyeingSlots::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CDyeingSlots* pInstance = new CDyeingSlots(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CDyeingSlots"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CDyeingSlots::Clone(void* pArg)
{
	CDyeingSlots* pInstance = new CDyeingSlots(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Created : CDyeingSlots"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDyeingSlots::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerInstance);

	for (auto& pSlot : m_Slots)
		Safe_Release(pSlot);
}
