#pragma once
#include "Client_Defines.h"
#include "UI_Panel.h"

NS_BEGIN(Client)

class CSlot;
class CPlayerInstance;

class CEquipment final : public CUI_Panel
{
private:
	enum EQUIPMENT {	EAR,	HEAD,	NECK,
						SWORD,	UPPER,	SHILED,
						TOOL,	LOWER,	HAND,
						BAND,	FOOT,	NONE, END};

private:
	CEquipment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEquipment(const CEquipment& Prototype);
	virtual ~CEquipment() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool	IsPick(HWND hWnd);
	virtual _bool	IsUnEquip(HWND hWnd);
private:
	CPlayerInstance*	m_pPlayerInstance = { nullptr };
	RECT				m_SlotRect = {};
	CSlot*				m_Slots[EQUIPMENT::END] = { nullptr };
	_uint				m_iIndexFlag = {};

	unordered_map<_uint, EVENT_PICK_EQUIPMENT>		m_PickEvents;
	unordered_map<_uint, EVENT_UNEQUIP_EQUIPMENT>	m_UnEqupEvents;
	unordered_map<WEAPON_TYPE, _uint>				m_WeaponSlotIndex;
	unordered_map<ARMOR_TYPE, _uint>				m_ArmorSlotIndex;

	_uint				m_iPickIndex = {};
	
	_uint				m_iNumSlotsX = {};
	_uint				m_iNumSlotsY = {};

	_float				m_fSlotStartOffsetX = {};
	_float				m_fSlotStartOffsetY = {};
	_float				m_fSlotOffsetX = {};
	_float				m_fSlotOffsetY = {};

private:
	HRESULT Ready_Children();
	HRESULT Ready_Slots();
	HRESULT Ready_Events();
	HRESULT Ready_SlotIndex();

	void	Event_Pick_Equip_Item(const EVENT_PICK_EQUIP_ITEM& Event);
	void	Event_Change_Weapon(const EVENT_CHANGE_WEAPON& Event);
	void	Event_Change_Armor(const EVENT_CHANGE_ARMOR& Event);

public:
	static CEquipment*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END