#include "ClientPch.h"
#include "Storage.h"
#include "Slot.h"

CStorage::CStorage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CUI_Panel{ pDevice, pDeviceContext }
	, m_pPlayerInstance { CPlayerInstance::GetInstance()}
{
	Safe_AddRef(m_pPlayerInstance);
}

CStorage::CStorage(const CStorage& Prototype)
	: CUI_Panel{ Prototype }
	, m_pPlayerInstance { Prototype.m_pPlayerInstance }
	, m_iNumSlotsX { Prototype.m_iNumSlotsX }
	, m_iNumSlotsY { Prototype.m_iNumSlotsY }
	, m_fSlotStartOffsetX { Prototype.m_fSlotStartOffsetX }
	, m_fSlotStartOffsetY { Prototype.m_fSlotStartOffsetY }
	, m_fSlotOffsetX { Prototype.m_fSlotOffsetX }
	, m_fSlotOffsetY { Prototype.m_fSlotOffsetY }
{
	Safe_AddRef(m_pPlayerInstance);
}

HRESULT CStorage::Initialize_Prototype()
{
	m_iNumSlotsX = g_iInventoryCount / 8;
	m_iNumSlotsY = g_iInventoryCount / 6;

	m_fSlotStartOffsetX = -122.5f;
	m_fSlotStartOffsetY = -190.f;

	m_fSlotOffsetX = 43.5f;
	m_fSlotOffsetY = 48.f;

	return S_OK;
}

HRESULT CStorage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Children()))
		return E_FAIL;

	m_pGameInstance->Subscribe<EVENT_PICK_ITEM>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_PICK_ITEM& Event) {
		this->Event_Pick_Item(Event); });

	m_pGameInstance->Subscribe<EVENT_UPDATE_INVENTORY>(ENUM_CLASS(EVENT_TYPE::STATIC), [this](const EVENT_UPDATE_INVENTORY& Event) {
		this->Event_Update_Inventory(Event); });

	return S_OK;
}

void CStorage::Priority_Update(_float fTimeDelta)
{
	__super::Children_Priority_Update(fTimeDelta);
}

void CStorage::Update(_float fTimeDelta)
{

	__super::Children_Update(fTimeDelta);
}

void CStorage::Late_Update(_float fTimeDelta)
{
	__super::Children_Late_Update(fTimeDelta);
}

HRESULT CStorage::Render()
{
	return S_OK;
}

HRESULT CStorage::Ready_Children()
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
	Children_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_GamePlay_Storage");
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::THIRD);
	Children_Desc.IsBlend = false;
	Children_Desc.fAlpha = 1.f;

	if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Panel"), &Children_Desc)))
		return E_FAIL;

	if (FAILED(Ready_Slots()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStorage::Ready_Slots()
{
	CUIObject::UIOBJECT_DESC Children_Desc = {};
	Children_Desc.fX = m_fX;
	Children_Desc.fY = m_fY;
	Children_Desc.fSizeX = 40.f;
	Children_Desc.fSizeY = 45.f;
	Children_Desc.iDepth = ENUM_CLASS(UI_DEPTH::FORTH);

	for (_uint i = 0; i < m_iNumSlotsY; i++)
	{
		for (_uint j = 0; j < m_iNumSlotsX; j++)
		{
			Children_Desc.fOffsetX = m_fSlotStartOffsetX + (m_fSlotOffsetX * static_cast<_float>(j));
			Children_Desc.fOffsetY = m_fSlotStartOffsetY + (m_fSlotOffsetY * static_cast<_float>(i));

			CSlot* pSlot = nullptr;

			if (FAILED(__super::Add_Child(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_UIObject_Slot"), &Children_Desc, reinterpret_cast<CUIObject**>(&pSlot))))
				return E_FAIL;

			m_Slots.push_back(pSlot);
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

_bool CStorage::IsPick(HWND hWnd)
{
	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	if(PtInRect(&m_SlotRect, ptMouse))
	{
		_float fX = static_cast<_float>(ptMouse.x - m_SlotRect.left);
		_float fY = static_cast<_float>(ptMouse.y - m_SlotRect.top);

		m_iPickIndex = static_cast<_uint>((fX / m_fSlotOffsetX)) + ((m_iNumSlotsX) * static_cast<_uint>(fY / m_fSlotOffsetY));

		EVENT_PICK_STORAGE Event = {};
		Event.iInventoryIndex = m_iPickIndex;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

		return true;
	}
	return false;
}

_bool CStorage::IsEquip(HWND hWnd)
{
	POINT	ptMouse = m_pGameInstance->Get_MousePoint();

	if (PtInRect(&m_SlotRect, ptMouse))
	{
		_float fX = static_cast<_float>(ptMouse.x - m_SlotRect.left);
		_float fY = static_cast<_float>(ptMouse.y - m_SlotRect.top);

		m_iPickIndex = static_cast<_uint>((fX / m_fSlotOffsetX)) + ((m_iNumSlotsX) * static_cast<_uint>(fY / m_fSlotOffsetY));

		EVENT_EQUIP_STORAGE Event = {};
		Event.iInventoryIndex = m_iPickIndex;
		m_pGameInstance->Publish(ENUM_CLASS(EVENT_TYPE::STATIC), Event);

		return true;
	}
	return false;
}

void CStorage::Event_Pick_Item(const EVENT_PICK_ITEM& Event)
{
	m_Slots[Event.iInventoryIndex]->IsPick(Event.IsPick);
}

void CStorage::Event_Update_Inventory(const EVENT_UPDATE_INVENTORY& Event)
{
	if (nullptr == Event.pIndices)
		return;

	for (_uint i = 0; i < Event.iNumIndices; i++)
		m_Slots[Event.pIndices[i]]->UpdateItem(m_pPlayerInstance->GetInventory(Event.pIndices[i]));
}


CStorage* CStorage::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CStorage* pInstance = new CStorage(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed Created : CStorage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CStorage::Clone(void* pArg)
{
	CStorage* pInstance = new CStorage(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CStorage"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStorage::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerInstance);

	for (auto& pSlot : m_Slots)
		Safe_Release(pSlot);

	m_Slots.clear();
}