#pragma once
#include "Projectile.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CGavelenRock final : public CProjectile
{
public:
	typedef struct tagGavelenRockDesc : public PROJECTILE_DESC
	{
		_bool* pIsSwing;
		const _float4x4* pSocketMatrixPtr;
		const _float4x4* pOwnerMatrixPtr;
	}GAVELEN_ROCK_DESC;

private:
	CGavelenRock(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGavelenRock(const CGavelenRock& Prototype);
	virtual ~CGavelenRock() = default;

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
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };
	_float4x4				m_CombinedMatrix = {};

	_bool*					m_pIsSwing = {};
	_bool					m_IsMove = {};
	const _float4x4*		m_pSocektMatrixPtr = { nullptr };
	const _float4x4*		m_pOwnerMatrixPtr = { nullptr };

private:
	HRESULT					Bind_ShaderResources();
	HRESULT					Ready_Component();
	void					Update_Bezier();

public:
	static CGavelenRock*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END