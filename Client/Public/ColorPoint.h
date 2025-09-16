#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CVIBuffer;
class CShader;
NS_END

NS_BEGIN(Client)

class CColorPoint final : public CUIObject
{
private:
	CColorPoint(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CColorPoint(const CColorPoint& Prototype);
	virtual ~CColorPoint() = default;

public:
	void			Set_Color(_float3 vColor) { m_vDyeingColor = vColor; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	CVIBuffer*	m_pVIBufferCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };

	_float3		m_vDyeingColor = {};

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CColorPoint*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END