#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(const _wstring& strLightTag)
{
	CLight* pLight = Find_Light(strLightTag);
	if (nullptr == pLight)
		return nullptr;

	return pLight->Get_LightDesc();
}

HRESULT CLight_Manager::Add_Light(const _wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	if (nullptr != Find_Light(strLightTag))
		return E_FAIL;

	CLight* pLight = CLight::Create(LightDesc);
	if (nullptr == pLight)
		return E_FAIL;

	m_Lights.emplace(strLightTag, pLight);

	return E_NOTIMPL;
}

CLight* CLight_Manager::Find_Light(const _wstring& strLightTag)
{
	auto Pair = m_Lights.find(strLightTag);
	if (Pair == m_Lights.end())
		return nullptr;

	return Pair->second;
}

CLight_Manager* CLight_Manager::Create()
{
	return new CLight_Manager();
}

void CLight_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_Lights)
		Safe_Release(Pair.second);

	m_Lights.clear();
}
