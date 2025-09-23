#pragma once
#include "Projectile.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Point_Instance;
class CShader;
NS_END

NS_BEGIN(Client)

class CEnergyBall final : public CProjectile
{
public:
	typedef struct tagEnergyBallDesc : public PROJECTILE_DESC
	{
		_vector StartPosition;
		_float	fDir;
	}ENERGYBALL_DESC;

private:
	CEnergyBall(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEnergyBall(const CEnergyBall& Prototype);
	virtual ~CEnergyBall() = default;

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
	_bool						m_IsMove = {};
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	_float						m_fDir;
	_float						m_fEffectTime = {};
	_float						m_fCurrentEffectTime = {};

	_vector						m_vMovePoints[4] = {};
	_float						m_fMinDistance = {};

private:
	HRESULT					Ready_Component();
	HRESULT					Bind_ShaderResources();
	void					Init_Bezier();
	void					Update_Bezier(_float fTimeDelta);

public:
	static CEnergyBall*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END