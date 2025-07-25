#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class CPawnObject;

class ENGINE_DLL CPawn abstract : public CGameObject
{
protected:
	CPawn(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPawn(const CPawn& Prototype);
	virtual ~CPawn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	map<const _wstring, CPawnObject*>		m_PawnObjects;

protected:
	HRESULT			Add_PawnObject(const _wstring& strPawnObjectTag, _uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, void* pArg = nullptr);
	CPawnObject*	Find_PawnObject(const _wstring& strPawnObjectTag);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END