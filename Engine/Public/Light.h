#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLight : public CBase
{
private:
	CLight();
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	const LIGHT_DESC* Get_LightDesc() { return &m_LightDesc; }
private:
	LIGHT_DESC		m_LightDesc = {};

public:
	static CLight*	Create(const LIGHT_DESC& LightDesc);
	virtual void	Free() override;
};

NS_END