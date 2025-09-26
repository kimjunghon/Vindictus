#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Dynamic final : public CVIBuffer
{
private:
	CVIBuffer_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Dynamic(const CVIBuffer_Dynamic& Prototype);
	virtual ~CVIBuffer_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Render() override;

public:
	void			Update_Buffer(vector<VTXPOSTEX>& Vertices);

private:
	D3D11_BUFFER_DESC		m_VBDesc = {};
	VTXPOSTEX*				m_pVertices = { nullptr };
	D3D11_BUFFER_DESC		m_IBDesc = {};
	_ushort*				m_pIndices = { nullptr };

	_uint					m_iCurrentIndices = {};

public:
	static CVIBuffer_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END