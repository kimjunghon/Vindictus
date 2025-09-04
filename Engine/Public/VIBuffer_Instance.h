#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint	iNumInstance;
		_float3	vCenter;
		_float3 vRange;
		_float2	vSize;
	}INSTANCE_DESC;

protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	virtual void	Update(_float fTimeDelta, _bool* pIsFinshed = nullptr);

protected:
	ID3D11Buffer*		m_pVBInstance = { nullptr };
	void*				m_pInstanceVertices = { nullptr };
	D3D11_BUFFER_DESC	m_VBInstanceDesc = {};
	_uint				m_iNumInstance = {};
	_uint				m_iNumIndexPerInstance = {};
	_uint				m_iInstanceVertexStride = {};

public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void		Free() override;
};

NS_END