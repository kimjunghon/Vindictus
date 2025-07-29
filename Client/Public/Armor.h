#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
class CBone;
NS_END

NS_BEGIN(Client)

class CArmor final : public CPawnObject
{
public:
	typedef struct tagArmorInfo {
		_float fDefense;
		_float fBrokenDefense;
		_float fHealth;
	}ARMOR_INFO;

	typedef struct tagArmorDesc : public PAWNOBJECT_DESC {
		_uint				iArmorModelPrototypeLevelIndex;
		_wstring			strArmorModelPrototypeTag;
		ARMOR_TYPE			eArmorType;
		ARMOR_INFO			tArmorInfo;
	}ARMOR_DESC;
	
private:
	CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CArmor(const CArmor& Prototype);
	virtual ~CArmor() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

	HRESULT					Equip(CModel* pParentModelCom);
	HRESULT					UnEquip();

protected:
	_bool					m_IsEquip = { false };
	ARMOR_TYPE				m_eArmorType = { ARMOR_TYPE::END };
	ARMOR_STATE				m_eArmorState = { ARMOR_STATE::DEFAULT };
	ARMOR_INFO				m_tArmorInfo = {};

	CModel*					m_pModelCom[ENUM_CLASS(ARMOR_STATE::END)] = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	vector<CBone*>			m_ParentBones;

private:
	HRESULT					Ready_Components(_uint iArmorNodelPrototypeLevelIndex, const _wstring& strArmorModelPrototypeTag);
	HRESULT					Bind_ShaderResources();
	HRESULT					Bind_ParentBones(CModel* pParentModelCom);

public:
	static CArmor*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;


};

NS_END