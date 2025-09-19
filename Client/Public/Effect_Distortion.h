#pragma once
#include "Effect.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer_Point_Instance;
class CShader;
NS_END

NS_BEGIN(Client)

class CEffect_Distortion final : public CEffect
{
public:
	typedef struct tagDistortionDesc : public EFFECT_DESC
	{
		_bool Is_Masking;
		_wstring strMaskTextureName;
	}DISTORTION_DESC;


private:
	CEffect_Distortion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CEffect_Distortion(const CEffect_Distortion& Prototype);
	virtual ~CEffect_Distortion() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	virtual void			Priority_Update(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual HRESULT			Spawn(void* pArg) override;


private:
	_bool						m_IsMasking = {};

	CTexture*					m_pDistortionTextureCom = { nullptr };
	CTexture*					m_pMaskTextureCom = { nullptr };
	CVIBuffer_Point_Instance*	m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };
	_bool						m_IsFinished = {};

private:
	HRESULT	Ready_Component();
	HRESULT Ready_MaskTexture(const _wstring& strMaskTextureName);
	HRESULT	Bind_ShaderResources();

public:
	static CEffect_Distortion*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END