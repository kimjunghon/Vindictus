#pragma once
#include "Client_Defines.h"
#include "PawnObject.h"

NS_BEGIN(Engine)
class CModel;
class CShader;
NS_END

NS_BEGIN(Client)

class CGlasgavelenSword final : public CPawnObject
{
private:
	CGlasgavelenSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CGlasgavelenSword(const CGlasgavelenSword& Prototype);
	virtual ~CGlasgavelenSword() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Priority_Update(_float fTimeDelta) override;
	virtual void	Update(_float fTimeDelta) override;
	virtual void	Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT			Bind_ParentBones(CModel* pParentModelCom);

private:
	CModel*					m_pModelCom = { nullptr };
	CShader*				m_pShaderCom = { nullptr };

	vector<CBone*>			m_ParentBones;
	
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGlasgavelenSword*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END