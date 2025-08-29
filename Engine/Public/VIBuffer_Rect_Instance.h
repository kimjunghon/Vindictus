#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagRectInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_bool	IsLoop;
	}RECT_INSTANCE_DESC;

private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

#ifdef _DEBUG
public:
	void			Update_Vertices(RECT_INSTANCE_DESC RectDesc);
	RECT_INSTANCE_DESC		Get_Desc() { return m_Desc; }

private:
	RECT_INSTANCE_DESC		m_Desc;
#endif
public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);

private:
	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};
	_bool					m_IsLoop = {};

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END