#pragma once
#include "Client_Defines.h"
#include "TextureUI.h"

NS_BEGIN(Engine)
class CShader;
class CVIBuffer;
NS_END

NS_BEGIN(Client)

class CPanel final : public CTextureUI
{
private:
	CPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPanel(const CPanel& Prototype);
	virtual ~CPanel() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CVIBuffer*		m_pVIBufferCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };

private:
	HRESULT			Ready_Components();

public:
	static CPanel*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END