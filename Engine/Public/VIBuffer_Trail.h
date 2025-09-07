#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
private:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& Prototype);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iNumMaxNode);
	virtual HRESULT Initialize(void* pArg) override;

	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	void			Clear();
	_bool			IsFinished();

public:
	void	Update(_float fTimeDelta);
	void	Update_TrailBuffer(TRAIL_NODE Trail, _float fTimeDelta);

	void	Update_TrailBuffer_Billboard(TRAIL_NODE Trail, _float fTimeDelta);

	_vector CatmullRom(_float3 vPoint0, _float3 vPoint1, _float3 vPoint2, _float3 vPoint3, _float fRatio);

private:
	deque<TRAIL_NODE>	m_Trails;
	_uint				m_iNumSample = {};
	_uint				m_iNumMaxNode = {};
	_uint				m_iNumCurrentNode = {};
	

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumMaxNode);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END