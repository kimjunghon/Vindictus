#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameInstance;
class CShader;
class CVIBuffer_Rect;

class CRenderer final : public CBase
{
private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
	HRESULT Draw();

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };
	CGameInstance*				m_pGameInstance = { nullptr };
	vector<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];

	CShader*					m_pShader = { nullptr };
	CVIBuffer_Rect*				m_pVIBuffer = { nullptr };
	_float4x4					m_WorldMatrix = {};
	_float4x4					m_ViewMatrix = {};
	_float4x4					m_ProjMatrix = {};

	ID3D11DepthStencilView*		m_pShadowDSV = { nullptr };

	_float						m_fViewportWidth = {};
	_float						m_fViewportHeight = {};

private:
	HRESULT Render_Priority();
	HRESULT Render_Shadow();
	HRESULT Render_NonBlend();
	HRESULT Render_Light();
	HRESULT Render_Combined();
	HRESULT Render_NonLight();
	HRESULT Render_Blend();
	HRESULT Render_UI();

private:
	HRESULT Ready_Shadow_Depth_Stencil_View();
	HRESULT SetUp_Viewport(_float fWidth, _float fHeight);

#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
private:
	list<class CComponent*>		m_DebugComponent;
	_bool						m_IsDebugDraw = {};
private:
	HRESULT Render_Debug();
#endif

public:
	static CRenderer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void		Free() override;
};

NS_END