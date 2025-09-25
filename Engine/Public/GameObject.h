#pragma once
#include "Transform.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct tagGameObjectDesc : public CTransform::TRANSFORM_DESC {
		
	}GAMEOBJECT_DESC;

protected:
	typedef map<const _wstring, class CComponent*> COMPONENTS;

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGameObject(const CGameObject& Prototype);
	virtual ~CGameObject() = default;

public:
	_float				Get_Depth();
	class CComponent*	Get_Component(const _wstring& strComponentTag);
	_bool				Is_Active() { return m_IsActive; }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void	Priority_Update(_float fTimeDelta);
	virtual void	Update(_float fTimeDelta);
	virtual void	Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_Shadow() { return S_OK; }

protected:
	_bool							m_IsActive = { true };
	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pDeviceContext = { nullptr };
	class CGameInstance*			m_pGameInstance = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	COMPONENTS						m_Components;

	_float							m_fDepth = {};

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag,
		const _wstring& strComponentTag, CComponent** ppOut, void* pArg = nullptr);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END