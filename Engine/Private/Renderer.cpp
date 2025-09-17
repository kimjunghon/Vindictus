#include "EnginePch.h"
#include "Renderer.h"
#include "GameInstance.h"
#include "UIObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice {pDevice}
	, m_pDeviceContext { pDeviceContext}
	, m_pGameInstance { CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderer::Initialize()
{
	_uint iNumViewports = { 1 };
	D3D11_VIEWPORT ViewportDesc = {};

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	m_fViewportWidth = ViewportDesc.Width;
	m_fViewportHeight = ViewportDesc.Height;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("RT_LightDepth"), g_iMaxWidth, g_iMaxHeight, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	///MRT_OBJECT///
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Object"), TEXT("RT_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Object"), TEXT("RT_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Object"), TEXT("RT_Depth"))))
		return E_FAIL;

	///MRT_LIGHT///
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Light"), TEXT("RT_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Light"), TEXT("RT_Specular"))))
		return E_FAIL;

	///MRT_SHADOW///
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("RT_LightDepth"))))
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	if (FAILED(Ready_Shadow_Depth_Stencil_View()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_Diffuse"), 150.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_Normal"), 150.0f, 350.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_Depth"), 150.0f, 600.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_Shade"), 400.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_Specular"), 650.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Ready_Debug(TEXT("RT_LightDepth"), 900.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
#endif

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
	if (FAILED(Render_Shadow()))
		return E_FAIL;
	if (FAILED(Render_NonBlend()))
		return E_FAIL;
	if (FAILED(Render_Light()))
		return E_FAIL;
	if (FAILED(Render_Combined()))
		return E_FAIL;
	if (FAILED(Render_NonLight()))
		return E_FAIL;
	if (FAILED(Render_Blend()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Render_Debug()))
		return E_FAIL;
#endif
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

HRESULT CRenderer::Render_Shadow()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
		return E_FAIL;

	if (FAILED(SetUp_Viewport(g_iMaxWidth, g_iMaxHeight)))
		return E_FAIL;

	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)])
	{
		if (nullptr != Object)
			Object->Render_Shadow();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::SHADOW)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(SetUp_Viewport(m_fViewportWidth, m_fViewportHeight)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Object"))))
		return E_FAIL;

	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONBLEND)].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Light()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Light"))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Normal"), m_pShader, "g_NormalTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;

	m_pGameInstance->Render_Light(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Combined()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Float4x4_Inverse(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ShadowLightViewMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::VIEW))))
		return E_FAIL;
	
	if (FAILED(m_pShader->Bind_Matrix("g_ShadowLightProjMatrix", m_pGameInstance->Get_ShadowLight_Transform_Float4x4(D3DTS::PROJ))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Shade"), m_pShader, "g_ShadeTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Specular"), m_pShader, "g_SpecularTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_Depth"), m_pShader, "g_DepthTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_Shader_RenderTarget(TEXT("RT_LightDepth"), m_pShader, "g_LightDepthTexture")))
		return E_FAIL;

	m_pShader->Begin(ENUM_CLASS(SHADER_DEFFERRED::COMBINED));

	m_pVIBuffer->Bind_Resources();
	m_pVIBuffer->Render();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pRenderObject : m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)])
	{
		if (nullptr != pRenderObject)
			pRenderObject->Render();

		Safe_Release(pRenderObject);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::NONLIGHT)].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	sort(m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].begin(), m_RenderObjects[ENUM_CLASS(RENDERGROUP::BLEND)].end(), [&](CGameObject* pSour, CGameObject* pDest) {
		return pSour->Get_Depth() < pDest->Get_Depth();
		});

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
	sort(m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].begin(), m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].end(), [&](CGameObject* pSour, CGameObject* pDest) {
		return static_cast<CUIObject*>(pSour)->Get_Depth() < static_cast<CUIObject*>(pDest)->Get_Depth();
		});
	
	for (auto& Object : m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)])
	{
		if (nullptr != Object)
			Object->Render();

		Safe_Release(Object);
	}

	m_RenderObjects[ENUM_CLASS(RENDERGROUP::UI)].clear();

	return S_OK;
}

HRESULT CRenderer::Ready_Shadow_Depth_Stencil_View()
{
	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iMaxWidth;
	TextureDesc.Height = g_iMaxHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pShadowDSV)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

HRESULT CRenderer::SetUp_Viewport(_float fWidth, _float fHeight)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = fWidth;
	ViewPortDesc.Height = fHeight;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	if (nullptr == pComponent)
		return E_FAIL;

	m_DebugComponent.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}

HRESULT CRenderer::Render_Debug()
{
	if (m_pGameInstance->Get_KeyDown(DIK_END))
		m_IsDebugDraw = !m_IsDebugDraw;

	if (false == m_IsDebugDraw)
	{
		for (auto& pDebugCom : m_DebugComponent)
		{
			Safe_Release(pDebugCom);
		}
		m_DebugComponent.clear();

		return S_OK;
	}

	for (auto& pDebugCom : m_DebugComponent)
	{
		if (nullptr != pDebugCom)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}
	m_DebugComponent.clear();


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	m_pGameInstance->Render_RT_Debug(m_pShader, m_pVIBuffer);

	return S_OK;
}
#endif

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

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShadowDSV);

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
}
