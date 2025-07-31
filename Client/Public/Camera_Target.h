#pragma once

#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Client)

class CCamera_Target final : public CCamera
{   
public:
	typedef struct tagCamera_Target_Dest : public CAMERA_DESC {
		const _float4x4*	TargetMatrix;
		_float				fDistance;
		_float				fHeight;
	}CAMERA_TARGET_DESC;

private:
	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCamera_Target(const CCamera_Target& Prototype);
	virtual ~CCamera_Target() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
public:
	void			Bind_InputData(const INPUT_CAMERA_DESC& CameraInput);
	_vector			Compute_PlayerMoveDir(_vector vInputDir);

private:
	const _float4x4*	m_TargetMatrix = {};
	_float				m_fDistance = {};
	_float				m_fMinDistance = {};
	_float				m_fMaxDistance = {};
	
	_float				m_fCurrentRotateY = {};
	_float				m_fHeight = {};

	_vector				m_vLook = {};
	_vector				m_vTargetPosition = {};
	_vector				m_vPosition = {};
	_float				m_fPitch = {};
	_float				m_fYaw = {};


public:
	static CCamera_Target*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END