#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Begin(_uint iPassIndex);

	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Bind_SPV(const _char* pConstantName, ID3D11ShaderResourceView* pSPV);
	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
	HRESULT Bind_RawValueArray(const _char* pConstantName, const void* pData, _uint iLength);

private:
	ID3DX11Effect*				m_pEffect = { nullptr };
	vector<ID3D11InputLayout*>	m_InputLayouts;
	_uint						m_iNumPasses = {};

public:
	static CShader*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void		Free() override;
};

NS_END