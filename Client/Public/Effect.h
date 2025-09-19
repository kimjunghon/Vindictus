#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CPool_Instance;

class CEffect abstract : public CGameObject
{
public:
	typedef struct tagEffectDesc {
		_uint iPassIndex;
		_uint iLevel;
		_wstring strEffectName;
	}EFFECT_DESC;

	typedef struct tagEffectSpawnDesc {
		_bool	IsEmissive = {};
		_matrix SpawnWorldMatrix;
	}EFFECT_SPAWN_DESC;

protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect(const CEffect& Prototype);
	virtual ~CEffect() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) PURE;
	virtual void			ReturnToPool();

protected:
	CPool_Instance*			m_pPool_Instance = { nullptr };
	_wstring				m_strEffectName = {};
	_uint					m_iPassIndex = {};
	_uint					m_iCurrentLevel = {};
	_bool					m_IsEmissive = {};

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END