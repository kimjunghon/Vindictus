#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
private:
	CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& Prototype);
	virtual ~CVIBuffer_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype(_uint iSphereX, _uint iSphereY);
	virtual HRESULT Initialize(void* pArg) override;

private:
	_uint	m_iSphereX = {};
	_uint	m_iSphereY = {};

public:
	static CVIBuffer_Sphere*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iSphereX, _uint iSphereY);
	virtual CComponent*			Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END