#pragma once
#include "VIBuffer_Instance.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Instance final : public CVIBuffer_Instance
{
public:
	enum class FX_POINT_TYPE { SPREAD, DROP, LOOP, FX_GRAVITY, END };

	typedef struct tagPointInstanceDesc : public INSTANCE_DESC
	{
		_float3 vPivot;
		_float2 vSpeed;
		_float2 vLifeTime;
		_bool	IsLoop;
		FX_POINT_TYPE eType;
	}POINT_INSTANCE_DESC;

private:
	CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype);
	virtual ~CVIBuffer_Point_Instance() = default;

public:
	void					Reset();
#ifdef _DEBUG
	POINT_INSTANCE_DESC		Get_Desc() { return m_Desc; }

private:
	POINT_INSTANCE_DESC		m_Desc;
#endif

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc) override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

	virtual void	Update(_float fTimeDelta, _bool* pIsFinshed = nullptr) override;

public:
	void Spread(_float fTimeDelta, _bool* pIsFinished);
	void Drop(_float fTimeDelta, _bool* pIsFinished);
	void Loop(_float fTimeDelta, _bool* pIsFinished);
	void Gravity(_float fTimeDelta, _bool* pIsFinished);
	void Circle(_float fTimeDelta, _bool* pIsFinished = nullptr);

	void Sort(_fmatrix WorldMatrix);

private:
	FX_POINT_TYPE			m_eType = {};
	_float3					m_vPivot = {};
	_float*					m_pSpeeds = {};
	_float					m_fCurrentGravity = {};
	_float					m_fCurrentTime = {};
	_bool					m_IsLoop = {};
	_uint					m_iNumCurrentInstance = {};

public:
	static CVIBuffer_Point_Instance*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc);
	virtual CComponent*					Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END