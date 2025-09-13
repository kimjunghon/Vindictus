#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTexture;
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CPalette final : public CUIObject
{
private:
	CPalette(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPalette(const CPalette& Prototype);
	virtual ~CPalette() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	ID3D11Texture2D*			m_pPaletteSample = { nullptr };

	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer*					m_pVIBufferCom = { nullptr };
	CShader*					m_pShaderCom = { nullptr };

	_uint						m_iDyeMaterial = {};

	_uint						m_iPaletteWidth = {};
	_uint						m_iPaletteHeight = {};

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_Palette();
	HRESULT			Bind_ShaderResources();

	_float			Noise(_float2 vUV);
	_float			FBM(_float2 vUV);
	_float3			HSV_To_RGB(_float3 vHSV);
	
public:
	static CPalette*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg);
	virtual void			Free() override;
};

NS_END