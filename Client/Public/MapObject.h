#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CMapObject final : public CGameObject
{
public:
	typedef struct tagMapObjectDesc : public GAMEOBJECT_DESC
	{
		_uint		iModelLevel;
		_wstring	strModelTag;
		_float4x4	WorldMatrix;
		_bool		IsNormal = {true};
		_bool		IsSpecular = {true};
		_bool		IsAmbient = {true};
	}MAP_OBJECT_DESC;

private:
	CMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMapObject(const CMapObject& Prototype);
	virtual ~CMapObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

	_bool			m_IsNormal = { };
	_bool			m_IsSpecular = { };
	_bool			m_IsAmbient = { };
private:
	HRESULT Ready_Components(_uint iModelLevel, const _wstring& strModelTag);
	HRESULT Bind_ShaderResources();
public:
	static CMapObject*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END