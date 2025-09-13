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
	void	Update_Billboard(_float fTimeDelta);

	void	Add_TrailBuffer(TRAIL_NODE Trail, _float fTimeDelta);
	void	Add_TrailBuffer_Billboard(TRAIL_NODE Trail, _float fTimeDelta);


private:
	D3D11_BUFFER_DESC		m_VBDesc = {};
	VTXTRAIL*				m_pVertices = { nullptr };
	D3D11_BUFFER_DESC		m_IBDesc = {};
	_ushort*				m_pIndices = { nullptr };

	deque<TRAIL_NODE>	m_Trails;
	_uint				m_iNumSample = {};
	_uint				m_iNumMaxNode = {};
	_uint				m_iNumCurrentNode = {};

private:
	void	Update_TrailBuffer(_float fTimeDelta);
	void	Update_TrailBuffer_Billboard(_float fTimeDelta);

	_vector CatmullRom(_float3 vPoint0, _float3 vPoint1, _float3 vPoint2, _float3 vPoint3, _float fRatio);
	void	Compute_CatmullRom_Billboard(_uint iNodeIndex, _float fRatio, _vector* pOut);

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumMaxNode);
	virtual CComponent*		Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END