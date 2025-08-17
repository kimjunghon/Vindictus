#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CNavigation;
NS_END

NS_BEGIN(Client)

class CMapObject;

class CMap final : public CGameObject
{
public:
	typedef struct tagMapDesc : public GAMEOBJECT_DESC
	{
		string strMapFilePath;
	}MAP_DESC;

private:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMap(const CMap& Prototype);
	virtual ~CMap() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CNavigation*		m_pNavigationCom = { nullptr };
	vector<CMapObject*> m_MapObjects;

private:
	HRESULT Ready_MapObjects(string strMapFilePath);
	HRESULT Ready_Components();

public:
	static CMap*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END