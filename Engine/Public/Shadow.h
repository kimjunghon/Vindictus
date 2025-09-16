#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;

class CShadow final : public CBase
{
private:
	CShadow();
	virtual ~CShadow() = default;

public:
	const _float4x4*	Get_Transform_Float4x4(D3DTS eTransformState) const { return &m_Matrices[ENUM_CLASS(eTransformState)]; }
	const _float*		Get_ShadowLight_Far() const { return &m_fShadowLightFar; }

public:
	HRESULT	Update_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc);
	void	Update();

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	SHADOW_LIGHT_DESC	m_ShadowLightDesc = {};
	_float				m_fShadowLightFar = {};
	_float4x4			m_Matrices[ENUM_CLASS(D3DTS::END)] = {};

private:
	_vector Compute_Center(const _float4* pWorldPoints);
	_matrix Compute_ShadowLight_Proj(const _float4* pWorldPoints);

public:
	static CShadow*		Create();
	virtual void		Free() override;
};

NS_END