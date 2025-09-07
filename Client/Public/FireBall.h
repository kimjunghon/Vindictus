#pragma once
#include "Projectile.h"

NS_BEGIN(Client)

class CFireBall final : public CProjectile
{
public:
	typedef struct tagFireBallDesc : public PROJECTILE_DESC
	{
		_bool* pIsSwing;
		const _float4x4* pSocketMatrixPtr;
		const _float4x4* pOwnerMatrixPtr;
	}FIREBALL_DESC;

private:
	CFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFireBall(const CFireBall& Prototype);
	virtual ~CFireBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;
	virtual void			ReturnToPool() override;

private:
	_bool*					m_pIsSwing = {};
	_bool					m_IsMove = {};
	const _float4x4*		m_pSocektMatrixPtr = { nullptr };
	const _float4x4*		m_pOwnerMatrixPtr = { nullptr };

	_float					m_fEffectTime = {};
	_float					m_fCurrentEffectTime = {};

private:
	HRESULT					Ready_Component();
	void					Update_Bezier();

public:
	static CFireBall*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END