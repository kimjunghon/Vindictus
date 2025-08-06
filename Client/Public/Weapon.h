#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Client)

class CWeapon final : public CPawnObject
{
public:
	typedef struct tagWeaponInfo {
		_wstring	strWeaponName;
		_float		fAttackPower;
		_float		fDefense;
	}WEAPON_INFO;

	typedef struct tagWeaponDesc : public PAWNOBJECT_DESC {
		_uint				iWeaponModelPrototypeLevelIndex;
		_wstring			strWeaponModelPrototypeTag;
		WEAPON_TYPE			eWeaponType;
		WEAPON_INFO			WeaponInfo;
		_float4				vRotationQuaternion;
	}WEAPON_DESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	const _wstring&			Get_WeaponName() { return m_WeaponInfo.strWeaponName; }
	WEAPON_TYPE				Get_WeaponType() { return m_eWeaponType; }
	HRESULT					Equip(const _float4x4* pParentMatrix, const _float4x4* pSocketMatrix);
	HRESULT					UnEquip();

private:
	const _float4x4*		m_pSocketMatrix = { nullptr };
	_float4x4				m_CombinedMatrix = { };

	_bool					m_IsEquip = { false };
	WEAPON_TYPE				m_eWeaponType = { WEAPON_TYPE::END };
	WEAPON_INFO				m_WeaponInfo = {};

	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	
private:
	HRESULT					Ready_Components(_uint iWeaponModelPrototypeLevelIndex, const _wstring& strWeaponModelPrototypeTag);
	HRESULT					Bind_ShaderResources();

public:
	static CWeapon*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END