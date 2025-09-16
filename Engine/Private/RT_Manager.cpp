#include "EnginePch.h"
#include "RT_Manager.h"
#include "RenderTarget.h"

CRT_Manager::CRT_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice }
	, m_pDeviceContext { pDeviceContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRT_Manager::Initialize()
{

	return S_OK;
}

HRESULT CRT_Manager::Add_RenderTarget(const _wstring& strRTTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	if (nullptr != Find_RenderTarget(strRTTag))
		return E_FAIL;

	CRenderTarget* pRT = CRenderTarget::Create(m_pDevice, m_pDeviceContext, iSizeX, iSizeY, ePixelFormat, vClearColor);
	if (nullptr == pRT)
		return E_FAIL;

	m_RenderTargets.emplace(strRTTag, pRT);

	return S_OK;
}

HRESULT CRT_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strRTTag)
{
	CRenderTarget* pRT = Find_RenderTarget(strRTTag);
	if (nullptr == pRT)
		return E_FAIL;

	list<CRenderTarget*>* pMRTs = Find_MRT(strMRTTag);
	if (nullptr == pMRTs)
	{
		list<CRenderTarget*> MRTs;
		MRTs.push_back(pRT);

		m_MRTs.emplace(strMRTTag, MRTs);
	}
	else
		pMRTs->push_back(pRT);

	Safe_AddRef(pRT);

	return S_OK;
}

HRESULT CRT_Manager::Bind_Shader_RenderTarget(const _wstring& strRTTag, CShader* pShader, const _char* pConstantName)
{
	CRenderTarget* pRT = Find_RenderTarget(strRTTag);
	if (nullptr == pRT)
		return E_FAIL;

	return pRT->Bind_ShaderResource(pShader, pConstantName);
}

HRESULT CRT_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool IsClear)
{
	list<CRenderTarget*>* pMRTs = Find_MRT(strMRTTag);
	if (nullptr == pMRTs)
		return E_FAIL;

	m_pDeviceContext->OMGetRenderTargets(1, &m_pBackBuffer, &m_pOriginDSV);

	_uint iNumRenderTargets = {};

	ID3D11RenderTargetView* RenderTargets[8] = { nullptr, };

	for (auto& pRT : *pMRTs)
	{
		if(IsClear)
			pRT->Clear();

		RenderTargets[iNumRenderTargets++] = pRT->Get_RTV();
	}

	if (pDSV)
		m_pDeviceContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pDeviceContext->OMSetRenderTargets(iNumRenderTargets, RenderTargets, nullptr == pDSV ? m_pOriginDSV : pDSV);

	return S_OK;
}

HRESULT CRT_Manager::End_MRT()
{
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBuffer, m_pOriginDSV);

	Safe_Release(m_pBackBuffer);
	Safe_Release(m_pOriginDSV);

	return S_OK;
}

CRenderTarget* CRT_Manager::Find_RenderTarget(const _wstring& strRTTag)
{
	auto iter = m_RenderTargets.find(strRTTag);
	if(iter == m_RenderTargets.end())
		return nullptr;
	
	return iter->second;
}

list<CRenderTarget*>* CRT_Manager::Find_MRT(const _wstring& strMRTTag)
{
	auto iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
		return nullptr;

	return &(iter->second);
}

#ifdef _DEBUG
HRESULT CRT_Manager::Ready_Debug(const _wstring& strRTTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	CRenderTarget* pRT = Find_RenderTarget(strRTTag);
	if (nullptr == pRT)
		return E_FAIL;

	return pRT->Ready_Debug(fX, fY, fSizeX, fSizeY);
}

HRESULT CRT_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRT : Pair.second)
		{
			pRT->Render(pShader, pVIBuffer);
		}
	}
	return S_OK;
}
#endif

CRT_Manager* CRT_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRT_Manager* pInstance = new CRT_Manager(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CRT_Manager"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRT_Manager::Free()
{
	__super::Free();

	for (auto& Pair : m_RenderTargets)
		Safe_Release(Pair.second);
	m_RenderTargets.clear();

	for (auto& Pair : m_MRTs)
	{
		for (auto& pRT : Pair.second)
		{
			Safe_Release(pRT);
		}
		Pair.second.clear();
	}
	m_MRTs.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
