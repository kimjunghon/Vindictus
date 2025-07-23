#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CRenderState final : public CBase
{
private:
	typedef map<_wstring, ID3D11RasterizerState*>	RS;
	typedef map<_wstring, ID3D11BlendState*>		BS;
private:
	CRenderState(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderState() = default;

public:
	HRESULT Initialize();

	HRESULT RSSetState(_uint iRSIndex);
	HRESULT RSSetState(const _wstring& strRSTag);

	HRESULT BSSetState(_uint iBSIndex);
	HRESULT BSSetState(const _wstring& strBSTag);

	HRESULT DSSSetState(_uint iDSSIndex);

	HRESULT Add_CustomRS(const _wstring& strRSTag, const D3D11_RASTERIZER_DESC& RS_DESC);
	HRESULT Add_CustomBS(const _wstring& strBSTag, const D3D11_BLEND_DESC& BS_DESC);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pDeviceContext = { nullptr };
	
	ID3D11RasterizerState*		m_RSs[ENUM_CLASS(D3DRS::END)] = { nullptr };
	RS							m_CustomRSs;
	
	ID3D11BlendState*			m_BSs[ENUM_CLASS(D3DBS::END)] = { nullptr };
	BS							m_CustomBSs;
	
	ID3D11DepthStencilState*	m_DSSs[ENUM_CLASS(D3DDSS::END)] = {nullptr};
	
private:
	HRESULT					Ready_RasterizerState();
	HRESULT					Ready_BlendState();
	HRESULT					Ready_DepthStencilState();
	ID3D11RasterizerState*	Find_RS(const _wstring& strRSTag);
	ID3D11BlendState*		Find_BS(const _wstring& strBSTag);

public:
	static CRenderState*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void			Free() override;
};

NS_END