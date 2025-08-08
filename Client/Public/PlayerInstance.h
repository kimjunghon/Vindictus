#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CWeapon;
class CArmor;

class CPlayerInstance : public CBase
{
	DECLARE_SINGLETON(CPlayerInstance)

private:
	CPlayerInstance();
	virtual ~CPlayerInstance() = default;

public:
	HRESULT			EquipWeapon(_uint iWeaponTypeIndex, CWeapon* pEquipWeapon);
	HRESULT			EquipArmor(_uint iArmorTypeIndex, CArmor* pEquipArmor);

	HRESULT			UnEquipWeapon(_uint iWeaponTypeIndex);
	HRESULT			UnEquipArmor(_uint iArmorTypeIndex);

	HRESULT			SavePlayerStatus(const PLAYER_STATUS& PlayerStatus);

	CWeapon*		BindPlayerEquipWeapon(_uint iWeaponTypeIndex);
	CArmor*			BindPlayerEquipArmor(_uint iArmorTypeIndex);
	PLAYER_STATUS	BindPlayerStatus();

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CWeapon*		m_pPlayerEquipWeapon[ENUM_CLASS(WEAPON_TYPE::END)] = { nullptr };
	CArmor*			m_pPlayerEquipArmor[ENUM_CLASS(ARMOR_TYPE::END)] = { nullptr };
	
	PLAYER_STATUS	m_PlayerStatus = {};
	
public:
	virtual void Free() override;
};

NS_END