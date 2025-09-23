#pragma once
#include "Client_Defines.h"
#include "Camera.h"

NS_BEGIN(Client)

class CCamera_CS final : public CCamera
{
public:
	typedef struct tagCameraCS_Desc: public CAMERA_DESC {
		const _char* pFilePath;
	}CAMERA_CS_DESC;

	typedef struct tagCameraCS_Reset_Desc {
		const _float4x4* pOwnerWorldMatrix;
		_matrix OffsetMatrix;
	}CAMERA_CS_RESET_DESC;

private:
	enum class CS_TYPE { STOP, LINEAR, SPLINE };

	enum class CS_EFFECT { NONE, SHAKE, ROTATE_NOISE };

	typedef struct tagCS_Data
	{
		_float fTime;
		_vector vPosition;
		_vector vRotation;
		CS_TYPE eType;
		CS_EFFECT eEffect;
	}CS_DATA;

private:
	CCamera_CS(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CCamera_CS(const CCamera_CS& Prototype);
	virtual ~CCamera_CS() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Reset(void* pArg) override;

private:
	const _float4x4*	m_pOwnerWorldMatrix = { nullptr };
	_matrix				m_OffsetMatrix = {};
	_float				m_fCurrentTime = {};
	_uint				m_iCurrentIndex = {};
	_uint				m_iNumNodes = {};
	vector<CS_DATA>		m_CS_Datas;

	_float				m_fCurrentEffectTime = {};

	_float				m_fNoiseFreqPitch = {};
	_float				m_fNoiseAmpPitch = {};
	_float				m_fNoiseFreqRoll = {};
	_float				m_fNoiseAmpRoll = {};

	_float				m_fShakeFreqPitch = {};
	_float				m_fShakeAmpPitch = {};
	_float				m_fShakeFreqRoll = {};
	_float				m_fShakeAmpRoll = {};

private:
	HRESULT Ready_Datas(const _char* pFilePath);
	void	Update_STOP();
	void	Update_LINEAR();
	void	Update_SPLINE();
	void	Find_Points(_vector* pPoints);

	void	Update_Shake(_float fTimeDelta);
	void	Update_RotateNoise(_float fTimeDelta);

public:
	static CCamera_CS*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END