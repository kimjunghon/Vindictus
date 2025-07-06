#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	typedef struct tagTransformDesc {
		_float	fSpeedPerSec;
		_float	fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTransform(const CTransform& Prototype) = default;
	virtual ~CTransform() = default;

public:
	_vector	Get_State(STATE eState) {
		return XMLoadFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]));
	}

	void Set_State(STATE eState, _fvector vState) {
		XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[ENUM_CLASS(eState)]), vState);
	}

	_float3 Get_Scaled() {
		return _float3(	XMVectorGetX(XMVector3Length(Get_State(STATE::RIGHT))),
						XMVectorGetX(XMVector3Length(Get_State(STATE::UP))),
						XMVectorGetX(XMVector3Length(Get_State(STATE::LOOK))));
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Shader_WorldMatrix(class CShader* pShader, const _char* pConstantWorldMatrixName);
	
public:
	void Scale(_float3 vScale);
	void Scaling(_float3 vScale);
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void Rotation(_fvector vAxis, _float fRadian);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void LookAt(_fvector vAt);
	void Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.f);

private:
	_float4x4			m_WorldMatrix = {};
	_float				m_fSpeedPerSec = {};
	_float				m_fRotationPerSec = {};
	_float3				m_vTotlaAngles = {};

public:
	static CTransform*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void		Free() override;
};

NS_END