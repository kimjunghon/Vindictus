#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect_Instance final : public CVIBuffer_Instance
{
public:
	enum class FX_RECT_TYPE { SPREAD, DROP, RING, END };

	typedef struct tagRectInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_bool	IsLoop;
		FX_RECT_TYPE eType;
	}RECT_INSTANCE_DESC;
	
private:
	CVIBuffer_Rect_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Rect_Instance(const CVIBuffer_Rect_Instance& Prototype);
	virtual ~CVIBuffer_Rect_Instance() = default;

public:
	void					Reset();
#ifdef _DEBUG
	RECT_INSTANCE_DESC		Get_Desc() { return m_Desc; }

private:
	RECT_INSTANCE_DESC		m_Desc;
#endif

public:
	
public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;

	virtual void	Update(_float fTimeDelta, _bool* pIsFinished = nullptr) override;

public:
	void Spread(_float fTimeDelta, _bool* pIsFinished = nullptr);
	void Drop(_float fTimeDelta, _bool* pIsFinished = nullptr);
	void Ring(_float fTimeDelta, _bool* pIsFinished = nullptr);
	void Circle(_float fTimeDelta, _bool* pIsFinished = nullptr);

private:
	FX_RECT_TYPE			m_eType = {};
	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};
	_bool					m_IsLoop = {};

public:
	static CVIBuffer_Rect_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*				Clone(void* pArg) override;
	virtual void					Free() override;
};

NS_END