#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CPlayerInstance;
class CWeapon;
class CArmor;

class CEquipment_Manager final : public CBase
{
private:
	CEquipment_Manager();
	virtual ~CEquipment_Manager() = default;

public:
	HRESULT					EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon, _int iItemInventoryIndex);
	HRESULT					EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor, _int iItemInventoryIndex);

	HRESULT					UnEquipWeapon(_uint iWeaponTypeIndex, _int iItemInventoryIndex = -1);
	HRESULT					UnEquipArmor(_uint iArmorTypeIndex, _int iItemInventoryIndex = -1);
	CWeapon*				UpdatePlayerEquipWeapon(_uint iWeaponTypeIndex) const;
	CArmor*					UpdatePlayerEquipArmor(_uint iArmorTypeIndex) const;

private:
	CPlayerInstance*	m_pPlayerInstance = { nullptr };
	CGameInstance*		m_pGameInstance = { nullptr };
	CWeapon*			m_pPlayerEquipWeapon[ENUM_CLASS(WEAPON_TYPE::END)] = { nullptr };
	CArmor*				m_pPlayerEquipArmor[ENUM_CLASS(ARMOR_TYPE::END)] = { nullptr };

public:
	static CEquipment_Manager* Create();
	virtual void Free();
};

NS_END