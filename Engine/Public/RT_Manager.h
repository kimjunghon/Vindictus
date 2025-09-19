#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CShader;
class CRenderTarget;

class CRT_Manager final : public CBase
{
private:
	CRT_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRT_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_RenderTarget(const _wstring& strRTTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const _wstring& strMRTTag, const _wstring& strRTTag);
	HRESULT Bind_Shader_RenderTarget(const _wstring& strRTTag, CShader* pShader, const _char* pConstantName);
	HRESULT Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV, _bool IsClear);
	HRESULT Clear_RT(const _wstring& strRTTag);
	HRESULT End_MRT();

private:
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pDeviceContext = { nullptr };
	ID3D11RenderTargetView*				m_pBackBuffer = { nullptr };
	ID3D11DepthStencilView*				m_pOriginDSV = { nullptr };

	map<_wstring, CRenderTarget*>		m_RenderTargets;
	map<_wstring, list<CRenderTarget*>> m_MRTs;

private:
	CRenderTarget*			Find_RenderTarget(const _wstring& strRTTag);
	list<CRenderTarget*>*	Find_MRT(const _wstring& strMRTTag);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(const _wstring& strRTTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif

public:
	static CRT_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void		Free() override;
};

NS_END