#include "Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice {pDevice}
	, m_pDeviceContext { pDeviceContext}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderer::Initialize()
{

	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	if (nullptr == pRenderObject)
		return E_FAIL;

	m_RenderObjects[ENUM_CLASS(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::PRIORITY)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();
	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].clear();
	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].clear();

	return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed Created : CRenderer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	__super::Free();

	for (size_t i = 0; i < ENUM_CLASS(RENDERGROUP::END); i++)
	{
		for (auto& pRenderObject : m_RenderObjects[i])
			Safe_Release(pRenderObject);

		m_RenderObjects[i].clear();
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
