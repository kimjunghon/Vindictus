#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class CNavigation;

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

	_matrix	Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}

	const _float4x4* Get_WorldMatrixPtr() {
		return &m_WorldMatrix;
	}

	_matrix Get_WorldMatrix_Inverse() {
		return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
	}

	void Set_WorldMatrix(_float4x4 WorldMatrix) {
		m_WorldMatrix = WorldMatrix;
	}

	void Set_WorldMatrix(_fmatrix WorldMatrix) {
		XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_Shader_WorldMatrix(class CShader* pShader, const _char* pConstantWorldMatrixName);
	
public:
	void	MovePositionToVector(_fvector vMovePosition, CNavigation* pNavigation);
	void	MovePositionToMatrix(_fmatrix PositionMatrix, CNavigation* pNavigation);
	void	Sliding(_fvector vDir, _fvector vNormal, CNavigation* pNavigation);

	void	Scale(_float3 vScale);
	void	Scaling(_float3 vScale);
	void	Go_Straight(_float fTimeDelta);
	void	Go_Backward(_float fTimeDelta);
	void	Go_Left(_float fTimeDelta);
	void	Go_Right(_float fTimeDelta);
	void	Rotation(_fvector vAxis, _float fRadian);
	void	RotateQuaternion(_fvector Quaternion);
	void	Turn(_fvector vAxis, _float fTimeDelta);
	void	LookAt(_fvector vAt);
	void	Chase(_fvector vTargetPos, _float fTimeDelta, _float fLimit = 0.f);


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