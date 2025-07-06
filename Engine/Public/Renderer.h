#pragma once
#include "Base.h"

NS_BEGIN(Engine)

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

	vector<class CGameObject*>	m_RenderObjects[ENUM_CLASS(RENDERGROUP::END)];

private:
	HRESULT Render_Priority();
	HRESULT Render_NonBlend();
	HRESULT Render_Blend();
	HRESULT Render_UI();

public:
	static CRenderer*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void		Free() override;
};

NS_END