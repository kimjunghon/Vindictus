#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	const _float4* Get_Frustum_WorldPoints() const { return m_vWorldPoints; }

public:
	HRESULT Initialize();
	void	Update();

private:
	CGameInstance*		m_pGameInstance = { nullptr };
	_float4				m_vPoints[8] = {};
	_float4				m_vWorldPoints[8] = {};
	_float4				m_vWorldPlanes[6] = {};

private:
	void	Update_WorldPlanes();

public:
	static CFrustum*	Create();
	virtual void		Free() override;
};

NS_END