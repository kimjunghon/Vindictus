#pragma once
#include "Effect.h"

NS_BEGIN(Client)

class CEffect_Prefab final : public CEffect
{
private:
	typedef struct tagEffectEvent {
		_wstring		strEffectName;
		_float			fSpawnTime;
		_matrix			OffsetMatrix;
	}PREFAB_EFFECT;

private:
	CEffect_Prefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Prefab(const CEffect_Prefab& Prototype);
	virtual ~CEffect_Prefab() = default;

public:
	virtual HRESULT			Initialize_Prototype(const Value& Effects);
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;

private:
	_float					m_fCurrentTime = {};
	_uint					m_iCurrentEffect = {};
	_uint					m_iNumMaxEffect = {};
	vector<PREFAB_EFFECT>	m_Effects;

public:
	static CEffect_Prefab*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Value& Effects);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END