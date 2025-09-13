#pragma once
#include "ClientPch.h"
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Trail;
class CShader;
NS_END

NS_BEGIN(Client)

class CEffect_Trail final : public CEffect
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
		_float				fNodeUpdateTime = {};
	}TRAIL_DESC;

private:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Trail(const CEffect_Trail& Prototype);
	virtual ~CEffect_Trail() = default;

public:
	virtual HRESULT			Initialize_Prototype(TRAIL_TYPE eType, _fmatrix RotateMatrix, _float3 vColor);
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
	TRAIL_TYPE			m_eType = {};

	_float4x4			m_CombinedMatrix = {};
	_float4x4			m_RotateMatrix = {};

	_float				m_fCurrentTime = {};
	_float				m_fNodeUpdateTime = {};
	const _float4x4*	m_pSocketMatrix = { nullptr };
	const _float4x4*	m_pParentMatrix = { nullptr };
	_bool*				m_IsSwing = { nullptr };
	_vector				m_vLeftPosition = {};
	_vector				m_vRightPosition = {};
	_float				m_fLifeTime = {};
	_float3				m_vColor = {};

private:
	HRESULT				Ready_Component();
	HRESULT				Bind_ShaderResources();


public:
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TRAIL_TYPE eType, _fmatrix RotateMatrix, _float3 vColor);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

NS_END