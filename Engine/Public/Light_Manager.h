#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CLight_Manager final: public CBase
{
public:
	typedef unordered_map<_wstring, class CLight*> LIGHTS;

private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHT_DESC*	Get_LightDesc(const _wstring& strLightTag);
	HRESULT				Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc);
	class CLight*		Find_Light(const _wstring& strLightTag);

private:
	LIGHTS		m_Lights;

public:
	static CLight_Manager*	Create();
	virtual void			Free() override;
};

NS_END