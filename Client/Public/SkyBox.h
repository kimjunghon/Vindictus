#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CTexture;
class CShader;
NS_END

NS_BEGIN(Client)

class CSkyBox final : public CGameObject
{
public:
	typedef struct tagSkyBoxDesc : public GAMEOBJECT_DESC
	{
		_uint	iTextureLevel;
		_wstring strTextureTag;
	}SKYBOX_DESC;

private:
	CSkyBox(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CSkyBox(const CSkyBox& Prototype);
	virtual ~CSkyBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	const _float4*	m_pCamPosition = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CVIBuffer*		m_pVIBufferCom = { nullptr };
	CTexture*		m_pTextureCom = { nullptr };

private:
	HRESULT		Ready_Component(_uint iTextureLevel, const _wstring& strTextureTag);
	HRESULT		Bind_ShaderResources();

public:
	static CSkyBox*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END