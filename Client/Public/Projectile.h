#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CCollider;
class CTransform;
NS_END

NS_BEGIN(Client)

class CPool_Instance;

class CProjectile abstract : public CGameObject
{
public:
	typedef struct tagProjectileDesc
	{
		ATTACK_TYPE eType;
		_float		fDamage;
		CTransform* pTargetTransform;
	}PROJECTILE_DESC;

protected:
	typedef _vector				 BEZIER_POINT;

	typedef struct tagLutDatas
	{
		_vector vPosition;
		_float fRatio;
		_float fLength;
	}LUT_DATA;

protected:
	CProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CProjectile(const CProjectile& Prototype);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) PURE;
	virtual void			ReturnToPool();

protected:
	CPool_Instance*			m_pPoolInstance = { nullptr };
	CCollider*				m_pColliderCom = { nullptr };
	CTransform*				m_pTargetTransformCom = { nullptr };
	
	_float					m_fDamage = {};
	ATTACK_TYPE				m_eType = {};

	vector<BEZIER_POINT>	m_Points;
	vector<LUT_DATA>		m_LUT;
	_float					m_fCurrentDistance = {};
	_float					m_fTotalDistance = {};
	_float					m_fSpeed = {};
	_bool					m_IsCurveFinished = {};
	_vector					m_vLastDir = {};

protected:
	virtual void	Ready_Bezier(_uint iNumPoints, _vector* pPointArray);
	_vector			Compute_CurvePosition(_float fRatio);
	_uint			Combination(_uint iN, _uint iR);
	virtual void	Ready_LUT();
	_vector			Compute_CurvePosition_LUT(_float fDistance);
	virtual void	Move(_float fTimeDelta);
	void			Move_Directional(_float fTimeDelta);
	void			Move_Curve(_float fTimeDelta);

public:
	virtual CGameObject*	Clone(void* pArg) PURE;
	virtual void			Free() override;
};

NS_END