#pragma once
#include "ClientPch.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Trail;
class CShader;
NS_END

NS_BEGIN(Client)

class CSwordTrail final : public CEffect
{
public:
	typedef struct tagTrailDesc 
	{
		const _float4x4*	pSocketMatrix;
		const _float4x4*	pParentMatrix;
		_bool*				IsSwing;
		_float3				vLeftPosition = {};
		_float3				vRightPosition = {};
		_float				fLifeTime = {};
	}TRAIL_DESC;

private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSwordTrail(const CSwordTrail& Prototype);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;
	virtual void			ReturnToPool() override;

	void					Clear();

private:
	CTexture*			m_pTextureCom = { nullptr };
	CVIBuffer_Trail*	m_pVIBufferCom = { nullptr };
	CShader*			m_pShaderCom = { nullptr };

	_float4x4			m_CombinedMatrix = {};
	_float4x4			m_RotateMatrix = {};

	const _float4x4*	m_pSocketMatrix = { nullptr };
	const _float4x4*	m_pParentMatrix = { nullptr };
	_bool*				m_IsSwing = { nullptr };
	_vector				m_vLeftPosition = {};
	_vector				m_vRightPosition = {};
	_float				m_fLifeTime = {};

private:
	HRESULT				Ready_Component();
	HRESULT				Bind_ShaderResources();


public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END