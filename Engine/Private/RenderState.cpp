#include "EnginePch.h"
#include "RenderState.h"

CRenderState::CRenderState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice { pDevice }
	, m_pDeviceContext { pDeviceContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderState::Initialize()
{
	if (FAILED(Ready_RasterizerState()))
		return E_FAIL;

	if (FAILED(Ready_BlendState()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilState()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderState::RSSetState(_uint iRSIndex)
{
	if (iRSIndex > ENUM_CLASS(D3DRS::END))
		return E_FAIL;

	m_pDeviceContext->RSSetState(m_RSs[iRSIndex]);

	return S_OK;
}

HRESULT CRenderState::RSSetState(const _wstring& strRSTag)
{
	ID3D11RasterizerState* pRS = Find_RS(strRSTag);
	if (nullptr == pRS)
		return E_FAIL;

	m_pDeviceContext->RSSetState(pRS);
	
	return S_OK;
}

HRESULT CRenderState::BSSetState(_uint iBSIndex)
{
	if (iBSIndex > ENUM_CLASS(D3DBS::END))
		return E_FAIL;

	m_pDeviceContext->OMSetBlendState(m_BSs[iBSIndex], nullptr, 0xFFFFFFFF);

	return S_OK;
}

HRESULT CRenderState::BSSetState(const _wstring& strBSTag)
{
	ID3D11BlendState* pBS = Find_BS(strBSTag);
	if (nullptr == pBS)
		return E_FAIL;

	m_pDeviceContext->OMSetBlendState(pBS, nullptr, 0xFFFFFFFF);

	return S_OK;
}

HRESULT CRenderState::DSSSetState(_uint iDSSIndex)
{
	if (iDSSIndex > ENUM_CLASS(D3DDSS::END))
		return E_FAIL;

	m_pDeviceContext->OMSetDepthStencilState(m_DSSs[iDSSIndex], 0);

	return S_OK;
}

HRESULT CRenderState::Add_CustomRS(const _wstring& strRSTag, const D3D11_RASTERIZER_DESC& RS_DESC)
{
	ID3D11RasterizerState* pRS = Find_RS(strRSTag);

	if (nullptr != pRS)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRasterizerState(&RS_DESC, &pRS)))
		return E_FAIL;

	m_CustomRSs.emplace(strRSTag, pRS);

	return S_OK;
}

HRESULT CRenderState::Add_CustomBS(const _wstring& strBSTag, const D3D11_BLEND_DESC& BS_DESC)
{
	ID3D11BlendState* pBS = Find_BS(strBSTag);

	if (nullptr != pBS)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBlendState(&BS_DESC, &pBS)))
		return E_FAIL;

	m_CustomBSs.emplace(strBSTag, pBS);

	return S_OK;
}

HRESULT CRenderState::Ready_RasterizerState()
{
	CD3D11_RASTERIZER_DESC DefaultDesc(D3D11_DEFAULT);

	m_pDevice->CreateRasterizerState(&DefaultDesc, &m_RSs[ENUM_CLASS(D3DRS::DEFAULT)]);

	DefaultDesc.FrontCounterClockwise = true;
	
	m_pDevice->CreateRasterizerState(&DefaultDesc, &m_RSs[ENUM_CLASS(D3DRS::CCW)]);

	DefaultDesc.FrontCounterClockwise = false;
	DefaultDesc.CullMode = D3D11_CULL_NONE;

	m_pDevice->CreateRasterizerState(&DefaultDesc, &m_RSs[ENUM_CLASS(D3DRS::NONCULL)]);

	DefaultDesc.CullMode = D3D11_CULL_BACK;
	DefaultDesc.FillMode = D3D11_FILL_WIREFRAME;
	
	m_pDevice->CreateRasterizerState(&DefaultDesc, &m_RSs[ENUM_CLASS(D3DRS::WIRE)]);

	return S_OK;
}

HRESULT CRenderState::Ready_BlendState()
{
	CD3D11_BLEND_DESC DefaultDesc(D3D11_DEFAULT);

	m_pDevice->CreateBlendState(&DefaultDesc, &m_BSs[ENUM_CLASS(D3DBS::DEFAULT)]);

	DefaultDesc.RenderTarget[0].BlendEnable = true;
	DefaultDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	DefaultDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	m_pDevice->CreateBlendState(&DefaultDesc, &m_BSs[ENUM_CLASS(D3DBS::ALPHABLEND)]);

	return S_OK;
}

HRESULT CRenderState::Ready_DepthStencilState()
{
	CD3D11_DEPTH_STENCIL_DESC DefaultDesc(D3D11_DEFAULT);

	m_pDevice->CreateDepthStencilState(&DefaultDesc, &m_DSSs[ENUM_CLASS(D3DDSS::DEFAULT)]);

	DefaultDesc.DepthEnable = false;
	DefaultDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	m_pDevice->CreateDepthStencilState(&DefaultDesc, &m_DSSs[ENUM_CLASS(D3DDSS::ZDISABLE)]);

	return S_OK;
}

ID3D11RasterizerState* CRenderState::Find_RS(const _wstring& strRSTag)
{
	auto Pair = m_CustomRSs.find(strRSTag);
	if (Pair == m_CustomRSs.end())
		return nullptr;

	return Pair->second;
}

ID3D11BlendState* CRenderState::Find_BS(const _wstring& strBSTag)
{
	auto Pair = m_CustomBSs.find(strBSTag);
	if (Pair == m_CustomBSs.end())
		return nullptr;

	return Pair->second;
}

CRenderState* CRenderState::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderState* pInstance = new CRenderState(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CRenderState"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderState::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

	for (_uint i = 0; i < ENUM_CLASS(D3DRS::END); i++)
		Safe_Release(m_RSs[i]);

	for (auto& Pair : m_CustomRSs)
		Safe_Release(Pair.second);
	m_CustomRSs.clear();

	for (_uint i = 0; i < ENUM_CLASS(D3DBS::END); i++)
		Safe_Release(m_BSs[i]);

	for (auto& Pair : m_CustomBSs)
		Safe_Release(Pair.second);
	m_CustomBSs.clear();

	for (_uint i = 0; i < ENUM_CLASS(D3DDSS::END); i++)
		Safe_Release(m_DSSs[i]);
}
